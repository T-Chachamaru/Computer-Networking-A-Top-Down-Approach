#import socket module
from socket import *
serverSocket = socket(AF_INET,SOCK_STREAM) 
serverPort = 12000
serverSocket.bind(('',serverPort))
serverSocket.listen(1)
while 1:     
    print('Ready to serve...')     
    connectionSocket,addr = serverSocket.accept()
    try:         
        message = connectionSocket.recv(1024).decode()
        filename = message.split()[1]
        f = open('C:\\Users\\kiin\\Desktop\\网络\\课程1\\' + filename[1:])
        outputdata = f.read()
        #Send one HTTP header line into socket         
        head = 'HTTP/1.1 200 OK\nConnection: close\nContent-Type: text/html\nContent-Length: %d\n\n' % (len(outputdata))
        connectionSocket.send(head.encode())

        #Send the content of the requested file to the client
        for i in range(0, len(outputdata)):
            connectionSocket.send(outputdata[i].encode())
        connectionSocket.close()
    except IOError:
        #Send response message for file not found
        head = 'HTTP/1.1 404 Not Found\n'
        connectionSocket.send(head.encode())

        #Close client socket
        connectionSocket.close()             