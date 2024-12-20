from socket import *
import time
serverName = '192.168.200.37'
serverPort = 12000
clientSocket = socket(AF_INET,SOCK_DGRAM)
clientSocket.settimeout(1)
message = 'ping'
minRtt = 100
maxRtt = 0
sumRtt = 0
loss = 0
for i in range(10):
    sendTime = time.time()
    messagesend = message + " " + str(i)
    try:
        clientSocket.sendto(messagesend.encode(),(serverName,serverPort))
        modifiedMessage,serverAddress = clientSocket.recvfrom(2048)
        Rtt = time.time() - sendTime
        if Rtt < minRtt:
            minRtt = Rtt
        if Rtt > maxRtt:
            maxRtt = Rtt
        sumRtt += Rtt
        modifiedMessage1 = modifiedMessage.decode() + " " + str(Rtt) + " " + serverName
        print(modifiedMessage1)
    except Exception as e:
        loss += 1
        print(f"{i} Request timed out")
print(f"minimum:{minRtt} maximum:{maxRtt} averageRtt:{sumRtt/10} packetlossrate:{loss/10*100}%")
clientSocket.close()