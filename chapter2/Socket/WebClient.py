import sys
from socket import *
sevrevName = sys.argv[1]
sevrevPort = int(sys.argv[2])
clientSocket = socket(AF_INET,SOCK_STREAM)
clientSocket.connect((sevrevName,sevrevPort))
openuri = sys.argv[3]
openhead = 'Get /' + openuri + ' HTTP/1.1\r\n'
clientSocket.send(openhead.encode())
message = clientSocket.recv(1024).decode()
print(message)
if int(message.split()[1]) == 200:
    for line in message.split('\n'):
        if line.startswith('Content-Length:'):
            contentlenght = int(line.split(':')[1])
    content = ''
    for i in range(contentlenght):
        content += clientSocket.recv(1024).decode()
    print(content)
else:
    print(message.split()[1]+message.split()[2])
clientSocket.close()