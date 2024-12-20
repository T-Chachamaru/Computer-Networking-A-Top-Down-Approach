from socket import *
import time

serverSocket = socket(AF_INET, SOCK_DGRAM)
serverSocket.settimeout(4)
serverSocket.bind(('', 12000))
times = 0
miss = 0
while 1:
    try:
        message,address = serverSocket.recvfrom(1024)
        message = message.decode()
        rTime = float(message.split()[1])
        endTime = rTime
        print(f"{times} {float(time.time()) - endTime}")
        miss = 0
    except Exception as e:
        miss += 1
        print(f"{times} miss")
        if miss >= 3:
            print("exit")
            break
    times += 1
serverSocket.close()