### 问题
In this lab, you will learn the basics of socket programming for UDP in Python. You will learn how to send and receive datagram packets using UDP sockets and also, how to set a proper socket timeout. Throughout the lab, you will gain familiarity with a Ping application and its usefulness in computing statistics such as packet loss rate.

You will first study a simple Internet ping server written in the Python, and implement a corresponding client. The functionality provided by these programs is similar to the functionality provided by standard ping programs available in modern operating systems. However, these programs use a simpler protocol, UDP, rather than the standard Internet Control Message Protocol (ICMP) to communicate with each other. The ping protocol allows a client machine to send a packet of data to a remote machine, and have the remote machine return the data back to the client unchanged (an action referred to as echoing). Among other uses, the ping protocol allows hosts to determine round-trip times to other machines. 

You are given the complete code for the Ping server below. Your task is to write the Ping client

### 代码
The following code fully implements a ping server. You need to compile and run this code before running your client program. You do not need to modify this code.

In this server code, 30% of the client’s packets are simulated to be lost. You should study this code carefully, as it will help you write your ping client.

The server sits in an infinite loop listening for incoming UDP packets. When a packet comes in and if a randomized integer is greater than or equal to 4, the server simply capitalizes the encapsulated data and sends it back to the client.

* [UDPPingerServer](UDPPingerServer.py)  

### 要求
You need to implement the following client program.
The client should send 10 pings to the server. Because UDP is an unreliable protocol, a packet sent from the client to the server may be lost in the network, or vice versa. For this reason, the client cannot wait indefinitely for a reply to a ping message. You should get the client wait up to one second for a reply; if no reply is received within one second, your client program should assume that the packet was lost during transmission across the network. You will need to look up the Python documentation to find out how to set the timeout value on a datagram socket.

Specifically, your client program should 
(1) send the ping message using UDP (Note: Unlike TCP, you do not need to establish a connection first, since UDP is a connectionless protocol.)
(2) print the response message from server, if any
(3) calculate and print the round trip time (RTT), in seconds, of each packet, if server responses 
(4) otherwise, print “Request timed out”

During development, you should run the UDPPingerServer.py on your machine, and test your client by sending packets to localhost (or, 127.0.0.1). After you have fully debugged your code, you should see how your application communicates across the network with the ping server and ping client running on different machines.

The ping messages in this lab are formatted in a simple way. The client message is one line, consisting of ASCII characters in the following format:

    Ping sequence_number time

where sequence_number starts at 1 and progresses to 10 for each successive ping message sent by the client, and time is the time when the client sends the message.

### 需要上交的内容
1. You will hand in the complete client code and screenshots at the client verifying that your ping program works as required.
2. Currently, the program calculates the round-trip time for each packet and prints it out individually. Modify this to correspond to the way the standard ping program works. You will need to report the minimum, maximum, and average RTTs at the end of all pings from the client. In addition, calculate the packet loss rate (in percentage).
3. Another similar application to the UDP Ping would be the UDP Heartbeat. The Heartbeat can be used to check if an application is up and running and to report one-way packet loss. The client sends a sequence number and current timestamp in the UDP packet to the server, which is listening for the Heartbeat (i.e., the UDP packets) of the client. Upon receiving the packets, the server calculates the time difference and reports any lost packets. If the Heartbeat packets are missing for some specified period of time, we can assume that the client application has stopped.Implement the UDP Heartbeat (both client and server). You will need to modify the given UDPPingerServer.py, and your UDP ping client.

### 答案
1. * [UDPPingerClient](UDPPingerClient.py)             
   * [PNG](UDPPingerClient.png)

2. * [UDPPingerClient2](UDPPingerClient2.py)
   * [PNG](UDPPingerClient2.png)

3. * [UDPHeartbeatClient](UDPHeartbeatClient.py)
   * [UDPHeartbeatServer](UDPHeartbeatServer.py)
   * [PNG](UDPHeartbeatServer.png)
