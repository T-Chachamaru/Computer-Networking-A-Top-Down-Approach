from socket import *
import time
serverName = '192.168.200.37'
serverPort = 12000
clientSocket = socket(AF_INET,SOCK_DGRAM)
clientSocket.settimeout(1)
message = 'ping'
for i in range(10):
    sendTime = time.time()
    messagesend = message + " " + str(i)
    try:
        clientSocket.sendto(messagesend.encode(),(serverName,serverPort))
        modifiedMessage,serverAddress = clientSocket.recvfrom(2048)
        Rtt = time.time() - sendTime
        modifiedMessage1 = modifiedMessage.decode() + " " + str(Rtt) + " " + serverName
        print(modifiedMessage1)
    except Exception as e:
        print(f"{i} Request timed out")
clientSocket.close()