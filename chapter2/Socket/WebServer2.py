from socket import *
import threading
def threadwebServer(connectionSocket):
    try:         
            message = connectionSocket.recv(1024).decode()
            print(message)
            filename = message.split()[1]
            f = open('C:\\Users\\kiin\\Desktop\\网络\\课程1\\' + filename[1:])
            outputdata = f.read()

            head = 'HTTP/1.1 200 OK\nConnection: close\nContent-Type: text/html\nContent-Length: %d\n\n' % (len(outputdata))
            connectionSocket.send(head.encode())

            for i in range(0, len(outputdata)):
                connectionSocket.send(outputdata[i].encode())
            connectionSocket.close()
    except IOError:
        head = 'HTTP/1.1 404 Not Found\n'
        connectionSocket.send(head.encode())
        connectionSocket.close()

serverSocket = socket(AF_INET,SOCK_STREAM) 
serverPort = 12000
serverSocket.bind(('',serverPort))
serverSocket.listen(10)
while 1:     
    print('Ready to serve...')     
    connectionSocket,addr = serverSocket.accept()
    thread = threading.Thread(target = threadwebServer,args = (connectionSocket,))
    thread.start()