from socket import *
import time
serverName = '192.168.200.37'
serverPort = 12000
clientSocket = socket(AF_INET,SOCK_DGRAM)
times = 0
while 1:
    message = "HeartBeat" + " " + str(time.time())
    clientSocket.sendto(message.encode(),(serverName,serverPort))
    print(times)
    times += 1
    time.sleep(3)
clientSocket.close()