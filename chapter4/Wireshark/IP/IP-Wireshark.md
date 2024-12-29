### 问题
#### A look at the captured trace
1. Select the first ICMP Echo Request message sent by your computer, and expand the Internet Protocol part of the packet in the packet details window. What is the IP address of your computer?
2. Within the IP packet header, what is the value in the upper layer protocol field?
3. How many bytes are in the IP header? How many bytes are in the payload of the IP datagram? Explain how you determined the number of payload bytes.
4. Has this IP datagram been fragmented? Explain how you determined whether or not the datagram has been fragmented.
5. Which fields in the IP datagram always change from one datagram to the next within this series of ICMP messages sent by your computer?
6. Which fields stay constant? Which of the fields must stay constant? Which fields must change? Why?
7. Describe the pattern you see in the values in the Identification field of the IP datagram
8. What is the value in the Identification field and the TTL field?
9. Do these values remain unchanged for all of the ICMP TTL-exceeded replies sent to your computer by the nearest (first hop) router? Why?
#### Fragmentation
10. Find the first ICMP Echo Request message that was sent by your computer after you changed the Packet Size in pingplotter to be 2000. Has that message been fragmented across more than one IP datagram? [Note: if you find your packet has not been fragmented, you should download the zip file http://gaia.cs.umass.edu/wireshark-labs/wireshark-traces.zip and extract the ip-ethereal-trace-1packet trace. If your computer has an Ethernet interface, a packet size of 2000 should cause fragmentation.3]
11. Print out the first fragment of the fragmented IP datagram. What information in the IP header indicates that the datagram been fragmented? What information in the IP header indicates whether this is the first fragment versus a latter fragment? How long is this IP datagram?
12. Print out the second fragment of the fragmented IP datagram. What information in the IP header indicates that this is not the first datagram fragment? Are the more fragments? How can you tell?
13. What fields change in the IP header between the first and second fragment?
14. How many fragments were created from the original datagram?
15. What fields change in the IP header among the fragments?

### 答案
1. 192.168.1.102。
2. ICMP(1)。
3. 报头20个字节。总长84个字节。有效负载64和字节。
4. 没有,片偏移字段为0。
5. Identification、Header checksum、TTL。
6. 版本、首部长度等字段是不会改变的。存活时间、校验和等字段是必须改变的。源IP与目的IP、片偏移等字段是可能改变的。
7. 在未被分片的ICMP回显请求消息中TTL超时所响应的报文中,Identification保持不变。在被分片的ICMP回显请求消息中TTL超时所响应的报文中,Identification针对整个分组响应不同的标识。
8. 0x9d7c,255。
9. 除了标识与校验和以外都保持不变,这主要是因为回显的TTL超时报文针对的目标一直是当前主机且没有额外参数改变。
10. 被分片了,以太网MTU值只有1500字节。
11. 第一个IP报文的总长字段为1500字节,这于以太网MTU吻合,flags也指明了该数据报被分片。其中片偏移字段为0显示此片段为第一个片段。
12. 观察flags字段中的值为not set,片偏移值为1480,总长只有548字节。可以确定此IP数据报是分片后的第二个报文,并且只被分片成了两个。
13. 主要是总长度、偏移、标识、数据以及校验和。
14. 通过观察flags字段,可以判断3500字节的ICMP回显请求消失被分片成了三个分段。
15. 主要是总长度、偏移、标识、数据以及校验和。