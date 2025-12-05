# Lab1

## 原代码分析

基础代码框架 `prog2.c` 是一个离散事件模拟器，用于模拟不可靠的网络层环境。

代码下半部分的模拟器核心通过维护一个事件链表来工作。主要事件类型包括：
* `FROM_LAYER5`: 应用层产生数据，调用发送方代码。
* `FROM_LAYER3`: 网络层将数据包投递给接收方（可能发生丢包、校验和错误）。
* `TIMER_INTERRUPT`: 定时器超时中断。

模拟器提供了以下不可靠信道特征：
* 丢包: 调用 `tolayer3` 时，数据包有一定概率直接消失。
* 位反转/损坏: 数据包的 payload、seqnum 或 acknum 可能被修改。
* 乱序与延迟: 模拟了网络传输时延。

需要实现的是传输层的逻辑，主要接口包括：
* 输出/输入: `A_output` (发送), `A_input` (收到ACK), `B_input` (收到数据)。
* 定时器管理: `A_timerinterrupt` (处理超时)。
* 初始化: `A_init`, `B_init`。

## 原代码中的历史遗留问题

由于原作者提供的 `prog2.c` 编写年代较早，在现代 GCC 编译器环境下存在兼容性问题。原代码在 `generate_next_arrival`, `starttimer`, `tolayer3` 等函数内部手动声明了 `char *malloc();`。现代 C 语言标准库 `<stdlib.h>` 将 `malloc` 定义为返回 `void *`。手动声明的 `char *` 与标准库冲突，导致编译报错 `conflicting types for ‘malloc’`。删除或注释掉所有函数内部的 `char *malloc();` 声明，直接依赖 `<stdlib.h>` 的标准定义。

## 功能设计

本次实验要求实现GBN协议，相较于停等协议，其核心特性如下：

1.  流水线发送: 允许发送方在未收到确认的情况下发送多个分组。
2.  累积确认: 接收方发送 ACK $n$，表示序号 $n$ 及之前的所有分组均已正确接收。
3.  单一接收窗口: 接收方只按序接收数据，丢弃任何乱序分组。
4.  单一相关定时器: 发送方只为最早的未确认分组维护一个定时器。

为了实现上述功能，需要在 A 端（发送方）和 B 端（接收方）维护独立的状态变量：

* A 端:
    * `base`: 发送窗口的基序号。
    * `next_seq_num`: 下一个待发送的序号。
    * `packet_buffer`: 缓存已发送但未确认，或等待发送的数据包。
* B 端:
    * `expected_seq_num`: 期望收到的下一个序号。
    * `last_ack_pkt`: 保存上一次发送的 ACK 包（用于重传）。

---

## 详细实现

### 校验和 (`calc_checksum`)
将 `seqnum`, `acknum` 和 `payload` 的每个字节作为整数进行累加。发送方计算并填充，接收方重新计算并比对。如果两者不一致，说明数据在传输层被物理层（模拟器）损坏。

### 初始化 (`A_init`, `B_init`)
* A 端：初始化 `base = 1`, `next_seq_num = 1`，清空缓冲区。
* B 端：初始化 `expected_seq_num = 1`，并准备一个默认的 ACK 包。

GBN 协议必须同步双方的初始序列号。这里约定从 1 开始。B 端准备 ACK 0 是为了处理一种边界情况，如果第 1 个包就出错，B 需要重传上一个正确接收的包的 ACK，即 ACK 0。

### 发送方逻辑 (`A_output`)
1.  缓冲区检查: 如果 `next_seq_num >= base + 50`，说明物理缓存已满，丢弃数据。
2.  打包: 将 Layer5 的消息封装成 Packet，计算 Checksum，存入 `packet_buffer`。
3.  窗口检查: 判断 `next_seq_num < base + WINDOW_SIZE`。
    * 在窗口内: 立即调用 `tolayer3` 发送。
    * 窗口已满: 仅保留在缓冲区中，不发送，等待窗口滑动。
4.  定时器逻辑: 如果 `base == next_seq_num` ，即发送的是窗口内的第一个包，启动定时器。

如果网络拥塞，新数据会被缓存在 Buffer 中，防止网络过载，同时保证可靠性。

### 发送方接收 ACK (`A_input`)
1.  校验: 检查 Checksum。
2.  累积确认处理: 如果 `packet.acknum >= base`：
    * 更新 `base = packet.acknum + 1`（窗口右移）。
    * 停止当前定时器。
    * 如果窗口内还有未确认包 (`base < next_seq_num`)，重启定时器。
3.  发送缓冲包: 循环检查缓冲区，将因窗口滑动而新进入窗口范围的数据包发送出去。

即使 ACK 1 丢了，只要收到 ACK 2，就意味着 1 和 2 都收到了，无需重传 1。定时器始终服务于 `base` 包，当 `base` 改变时，必须为新的 `base` 重新计时。窗口滑动后，不仅要处理确认，还要检查是否有“积压在缓冲区”的数据现在可以发送了。

### 超时重传 (`A_timerinterrupt`)
1.  启动定时器。
2.  循环遍历从 `base` 到 `next_seq_num` 的所有包。
3.  调用 `tolayer3` 重传这些包。

一旦发生超时，协议假设 `base` 包丢失。由于接收方不缓存乱序包，`base` 之后发出的包，即使到达了 B 端也会被 B 丢弃。因此 A 必须重传窗口内所有已发送的包。

### 接收方逻辑 (`B_input`)
1.  校验: 检查 Checksum。若出错，重传 `last_ack_pkt`。
2.  顺序检查:
    * 如果 `packet.seqnum == expected_seq_num`:
        * 交付数据给 Layer 5。
        * 发送 ACK `expected_seq_num`。
        * `expected_seq_num++`。
        * 更新 `last_ack_pkt`。
    * 如果乱序 (`!= expected`):
        * 丢弃数据包。
        * 重传 `last_ack_pkt`，即对上一个有序包的确认。

接收方不维护缓存，不处理乱序，只索要它期望的下一个包。
