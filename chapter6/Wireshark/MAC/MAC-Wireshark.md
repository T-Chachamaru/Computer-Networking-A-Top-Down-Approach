### 问题
#### Capturing and analyzing Ethernet frames
1. What is the 48-bit Ethernet address of your computer?
2. What is the 48-bit destination address in the Ethernet frame? Is this the Ethernet address of gaia.cs.umass.edu? (Hint: the answer is no). What device has this as its Ethernet address?
3. Give the hexadecimal value for the two-byte Frame type field. What upper layer protocol does this correspond to?
4. How many bytes from the very start of the Ethernet frame does the ASCII “G” in “GET” appear in the Ethernet frame?
5. What is the value of the Ethernet source address? Is this the address of your computer, or of gaia.cs.umass.edu (Hint: the answer is no). What device has this as its Ethernet address?
6. What is the destination address in the Ethernet frame? Is this the Ethernet address of your computer?
7. Give the hexadecimal value for the two-byte Frame type field. What upper layer protocol does this correspond to?
8. How many bytes from the very start of the Ethernet frame does the ASCII “O” in “OK” (i.e., the HTTP response code) appear in the Ethernet frame?
#### The Address Resolution Protocol
9. Write down the contents of your computer’s ARP cache. What is the meaning of each column value?
10. What are the hexadecimal values for the source and destination addresses in the Ethernet frame containing the ARP request message?
11. Give the hexadecimal value for the two-byte Ethernet Frame type field. What upper layer protocol does this correspond to?
12. Download the ARP specification fromftp://ftp.rfc-editor.org/in-notes/std/std37.txt. A readable, detailed discussion of ARP is also at http://www.erg.abdn.ac.uk/users/gorry/course/inet-pages/arp.html. <br>
a) How many bytes from the very beginning of the Ethernet frame does the ARP opcode field begin? <br>
b) What is the value of the opcode field within the ARP-payload part of the Ethernet frame in which an ARP request is made?<br>
c) Does the ARP message contain the IP address of the sender?<br>
d) Where in the ARP request does the “question” appear – the Ethernet address of the machine whose corresponding IP address is being queried?<br>
13. Now find the ARP reply that was sent in response to the ARP request. <br>
a) How many bytes from the very beginning of the Ethernet frame does the ARP opcode field begin? <br>
b) What is the value of the opcode field within the ARP-payload part of the Ethernet frame in which an ARP response is made?<br>
c) Where in the ARP message does the “answer” to the earlier ARP request appear – the IP address of the machine having the Ethernet address whose corresponding IP address is being queried?<br>
14. What are the hexadecimal values for the source and destination addresses in the Ethernet frame containing the ARP reply message?
15. Open the ethernet-ethereal-trace-1 trace file in http://gaia.cs.umass.edu/wireshark-labs/wireshark-traces.zip. The first and second ARP packets in this trace correspond to an ARP request sent by the computer running Wireshark, and the ARP reply sent to the computer running Wireshark by the computer with the ARP-requested Ethernet address. But there is yet another computer on this network, as indicated by packet 6 – another ARP request. Why is there no ARP reply (sent in response to the ARP request in packet 6) in the packet trace?

### 答案
1. 00:d0:59:a9:3d:68。
2. 00:06:25:da:af:73,这不是目标网址的MAC地址,这是默认网关路由器的MAC地址,用来接入互联网以发送至目标IP。
3. 0800,这表示为IPv4协议。
4. 54个字节。
5. 00:06:25:da:af:73,这是默认网关路由器的MAC地址。
6. 00:d0:59:a9:3d:68,这是我的计算机的MAC地址。
7. 0800,这表示为IPv4协议。
8. 67个字节。
9. 显示了多个接口的ARP缓存,其中有三个列项,分别代表IP地址、物理地址、类型(静态或动态)。
10. 源地址是00:d0:59:a9:3d:68,目标地址是ff:ff:ff:ff:ff:ff,正在运用MAC广播。
11. 0806,表示ARP协议。
12. a) 20个字节。<br>
    b) 值为1。<br>
    c) 包含了发送人的ARP地址。<br>
    d) 出现在Target IP address字段
13. a) 20个字节。<br>
    b) 值为2。<br>
    c) 出现了答案,Sender MAC address字段的值00:06:25:da:af:73为返回的答案。
14. 源地址为00:06:25:da:af:73,目标地址为00:d0:59:a9:3d:68。
15. 原因有很多,其中一种可能是当前子网没有此IP地址的主机。