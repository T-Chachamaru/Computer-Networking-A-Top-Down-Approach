from socket import *
import os
import sys
import struct
import time
import select
import binascii

ICMP_ECHO_REQUEST = 8
def checksum(string):
    csum = 0
    countTo = (len(string) // 2) * 2
    count = 0
    while count < countTo:
        thisVal = string[count + 1] * 256 + string[count]
        csum = csum + thisVal
        csum = csum & 0xffffffff
        count = count + 2

    if countTo < len(string):
        csum = csum + string[len(string) - 1]
        csum = csum & 0xffffffff
    csum = (csum >> 16) + (csum & 0xffff)
    csum = csum + (csum >> 16)
    answer = ~csum
    answer = answer & 0xffff
    answer = answer >> 8 | (answer << 8 & 0xff00)
    return answer


def receiveOnePing(mySocket, ID, timeout, destAddr):
    timeLeft = timeout
    startedSelect = time.time()
    whatReady = select.select([mySocket], [], [], timeLeft)
    howLongInSelect = (time.time() - startedSelect)
    if whatReady[0] == []:  # Timeout
        print("Request timed out.")
        return None
    timeReceived = time.time()
    recPacket, addr = mySocket.recvfrom(1024)

    leftbytes = recPacket[:-8]
    icmpheader = leftbytes[-8:]
    type,code,checksum,id,seq = struct.unpack("!bbHHh",icmpheader)
    if type != 0 or id != ID:
        print("Echo Reply failed")
        return None
    else:
        ipheader = leftbytes[:20]
        icmpdata_bytes = struct.calcsize("d")
        ttl = struct.unpack("!b",ipheader[8:9])[0]
        timeSend = struct.unpack("!d",recPacket[28:])[0]
        delay = timeReceived - timeSend
        return (icmpdata_bytes,delay,ttl)

def sendOnePing(mySocket, destAddr, ID,seq):
    # Header is type (8), code (8), checksum (16), id (16), sequence (16)
    myChecksum = 0
    # Calculate the checksum on the data and the dummy header. myChecksum = checksum(str(header + data))
    # Get the right checksum, and put in the header
    header = struct.pack("!bbHHh", ICMP_ECHO_REQUEST, 0, myChecksum, ID, seq)
    data = struct.pack("!d", time.time())
    myChecksum = checksum(header+data)
    header = struct.pack("!bbHHh", ICMP_ECHO_REQUEST, 0, myChecksum, ID, seq)
    packet = header + data
    mySocket.sendto(packet, (destAddr, 1))  # AF_INET address must be tuple, not str # Both LISTS and TUPLES consist of a number of objects
    # which can be referenced by their position number within the object.


def doOnePing(destAddr, timeout,seq):
    icmp = getprotobyname("icmp")
    # SOCK_RAW is a powerful socket type. For more details:	http://sock- raw.org/papers/sock_raw
    mySocket = socket(AF_INET, SOCK_RAW, icmp)
    myID = os.getpid() & 0xFFFF  # Return the current process i
    sendOnePing(mySocket, destAddr, myID, seq)
    delay = receiveOnePing(mySocket, myID, timeout, destAddr)
    mySocket.close()
    return delay


def ping(host, timeout=1):
    # timeout=1 means: If one second goes by without a reply from the server,
    # the client assumes that either the client's ping or the server's pong is lost
    dest = gethostbyname(host)
    print("\nPinging " + dest + " using Python:")
    print("")
    loss = timeavg = 0;totalsend = 5
    minDelay = 1000
    maxDelay = 0
    # Send ping requests to a server separated by approximately one second
    for i in range(0,totalsend):
        recv = doOnePing(dest, timeout,i)
        if recv == None:
            loss += 1
        else:
            tmptime = int(recv[1]*1000)
            print("From {:} reply: bytes={:} delay={:}ms TTL={:}".format(dest,recv[0],tmptime,recv[2]))
            minDelay = min(tmptime,minDelay);maxDelay = max(tmptime,maxDelay)
            timeavg += tmptime
        time.sleep(1)  # one second
    print("\n{:} Ping Statistic:".format(dest))
    print("\tData Packet: Sended = {:}, Recved = {:}, Loss = {:}({:}% Loss),".format(totalsend,totalsend-loss,loss,(int)(loss/totalsend*100)))
    print("Delay Estimated Time(ms):\n\tShortest = {:}ms, Longest = {:}ms, aver = {:}ms".format(minDelay,maxDelay,timeavg//totalsend))

ping('goole.com')