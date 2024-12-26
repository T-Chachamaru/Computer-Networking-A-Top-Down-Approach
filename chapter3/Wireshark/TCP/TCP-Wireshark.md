### 问题
#### A first look at the captured trace
1. What is the IP address and TCP port number used by the client computer (source) that is transferring the file to gaia.cs.umass.edu? To answer this question, it’s probably easiest to select an HTTP message and explore the details of the TCP packet used to carry this HTTP message, using the “details of the selected packet header window” refer to Figure 2 in the “Getting Started with Wireshark” Lab if you’re uncertain about the Wireshark windows.
2. What is the IP address of gaia.cs.umass.edu? On what port number is it sending and receiving TCP segments for this connection?
3. What is the IP address and TCP port number used by your client computer (source) to transfer the file to gaia.cs.umass.edu?
#### TCP Basics
4. What is the sequence number of the TCP SYN segment that is used to initiate the TCP connection between the client computer and gaia.cs.umass.edu? What is it in the segment that identifies the segment as a SYN segment?
5. What is the sequence number of the SYNACK segment sent by gaia.cs.umass.edu to the client computer in reply to the SYN? What is the value of the Acknowledgement field in the SYNACK segment? How did gaia.cs.umass.edu determine that value? What is it in the segment that identifies the segment as a SYNACK segment?
6. What is the sequence number of the TCP segment containing the HTTP POST command? Note that in order to find the POST command, you’ll need to dig into the packet content field at the bottom of the Wireshark window, looking for a segment with a “POST” within its DATA field.
7. Consider the TCP segment containing the HTTP POST as the first segment in the TCP connection. What are the sequence numbers of the first six segments in the TCP connection (including the segment containing the HTTP POST)? At what time was each segment sent? When was the ACK for each segment received? Given the difference between when each TCP segment was sent, and when its acknowledgement was received, what is the RTT value for each of the six segments? What is the EstimatedRTT value (see Section 3.5.3, page 242 in text) after the receipt of each ACK? Assume that the value of the EstimatedRTT is equal to the measured RTT for the first segment, and then is computed using the EstimatedRTT equation on page 242 for all subsequent segments.
Note: Wireshark has a nice feature that allows you to plot the RTT for each of the TCP segments sent. Select a TCP segment in the “listing of captured packets” window that is being sent from the client to the gaia.cs.umass.edu server. Then select: Statistics->TCP Stream Graph->Round Trip Time Graph.
8. What is the length of each of the first six TCP segments?
9. What is the minimum amount of available buffer space advertised at the received for the entire trace? Does the lack of receiver buffer space ever throttle the sender?
10. Are there any retransmitted segments in the trace file? What did you check for (in the trace) in order to answer this question?
11. How much data does the receiver typically acknowledge in an ACK? Can you identify cases where the receiver is ACKing every other received segment (see Table 3.2 on page 250 in the text).
12. What is the throughput (bytes transferred per unit time) for the TCP connection? Explain how you calculated this value.
#### TCP congestion control in action
13. Use the Time-Sequence-Graph(Stevens) plotting tool to view the sequence number versus time plot of segments being sent from the client to the gaia.cs.umass.edu server. Can you identify where TCP’s slowstart phase begins and ends, and where congestion avoidance takes over? Comment on ways in which the measured data differs from the idealized behavior of TCP that we’ve studied in the text.
14. Answer each of two questions above for the trace that you have gathered when you transferred a file from your computer to gaia.cs.umass.edu

### 答案
1. 源IP为10.10.128.192与源端口号54454。
2. gaia.cs.umass.edu的地址是128.119.245.12,在端口80接收与发送连接。
3. 如题1。
4. 序列号为0,NAK为1,表示请求连接功能,进入握手阶段。
5. 序列号为0,NAK为1,ACK为1,Acknowledgment栏位的值也是1,表示接受到连接请求,回送包确认,进入握手阶段的第二步。
6. 有POST字段的TCP报文序列号为1,进入握手阶段的第三步，同时置PSH位发送数据。
7. 前六个区段的序列号分别为1,754,2128,3502,4876,6250。每区段发送的时间分别是第一个为2.066054,剩下五个都是2.066185,能观察出浏览器采用流水线发送模式，同时发送了多个包。接着至2.289242接收到一个累积确认的ACK,能测算出RTT约为0.2秒。
8. 前六个TCP区段的长度,第一个为753,剩下五个为1374。
9. TCP连接在执行第三次握手后,同一时间发送了9个包,因此可以测算出最小缓冲区长度为9MSS。
10. 没有发现重传的区段。
11. 接收方在接收了9个数据报后进行累积确认,确认号为13120,窗口大小为55552。
12. 吞吐量=数据传输大小/所用时间,因此发送总bytes数95517/所耗时间2.789947约为35000bytes。
13. 略。
14. 略。