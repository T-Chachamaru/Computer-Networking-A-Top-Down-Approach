from socket import *
import base64
import ssl

msg = "\r\n I love computer networks!"
endmsg = "\r\n.\r\n"
mailserver = "smtp.163.com"
mailFrom = '*********@163.com'
mailTo = '*********@foxmail.com'
passwd = '*********'
mailBase64 = base64.b64encode(mailFrom.encode()).decode()
passwdBase64 = base64.b64encode(passwd.encode()).decode()

context = ssl.create_default_context()

clientSocket = socket(AF_INET,SOCK_STREAM)
clientSocket.connect((mailserver,465))
clientSocketSSL = context.wrap_socket(clientSocket,server_hostname=mailserver)

recv = clientSocketSSL.recv(1024).decode()
heloCommand = 'HELO Alice\r\n'
clientSocketSSL.send(heloCommand.encode())
recv1 = recv + clientSocketSSL.recv(1024).decode()
print(recv1)
if recv1[:3] != '220':
    print('220 reply not received from server.')

antuMsg = 'AUTH LOGIN\r\n'
clientSocketSSL.send(antuMsg.encode())
recv2 = clientSocketSSL.recv(1024).decode()
print(recv2)
if recv2[:3] != '250':
    print('250 reply not received from server.')

clientSocketSSL.send(str(mailBase64 + '\r\n').encode())
recv3 = clientSocketSSL.recv(1024).decode()
print(recv3)
if recv3[:3] != '334':
    print('334 reply not received from server.')

clientSocketSSL.send(str(passwdBase64 + '\r\n').encode())
recv4 = clientSocketSSL.recv(1024).decode()
print(recv4)
if recv4[:3] != '334':
    print('334 reply not received from server.')

clientSocketSSL.send(str('MAIL FROM:<' + mailFrom + '>\r\n').encode())
recv5 = clientSocketSSL.recv(1024).decode()
print(recv5)
if recv5[:3] != '235':
    print('235 reply not received from server.')

clientSocketSSL.send(str('RCPT TO:<' + mailTo + '>\r\n').encode())
recv6 = clientSocketSSL.recv(1024).decode()
print(recv6)
if recv6[:3] != '250':
    print('250 reply not received from server.')

clientSocketSSL.send('DATA\r\n'.encode())
recv7 = clientSocketSSL.recv(1024).decode()
print(recv7)
if recv7[:3] != '354':
    print('Expected 354 reply, got:', recv7)
else:
    boundary = "simpleboundary123"
    text_part = (
        f"From: {mailFrom}\r\n"
        f"To: {mailTo}\r\n"
        "Subject: hello, you!\r\n"
        f"Content-Type: multipart/mixed; boundary=\"{boundary}\"\r\n\r\n"
        f"--{boundary}\r\n"
        "Content-Type: text/html; charset=\"utf-8\"\r\n"
        "Content-Transfer-Encoding: 7bit\r\n\r\n"
        "<h1>hello</h1>\r\n"
        f"--{boundary}\r\n"
    )
    text_part = text_part.encode()

with open("E:\github\Learning logs\Computer-Networking-A-Top-Down-Approach\chapter2\Socket\SMTPClient-Socket\Melina.jpg", "rb") as img_file:
        img_data = img_file.read()
        img_b64 = base64.b64encode(img_data).decode()
        img_part = (
            f"Content-Type: image/jpeg; name=\"Melina.jpg\"\r\n"
            "Content-Transfer-Encoding: base64\r\n"
            "Content-Disposition: inline; filename=\"Melina.jpg\"\r\n\r\n"
            f"{img_b64}\r\n"
            f"--{boundary}--\r\n"
        )
        img_part = img_part.encode()

clientSocketSSL.send(text_part + img_part)
clientSocketSSL.send(b".\r\n")
recv8 = clientSocketSSL.recv(1024).decode()
print(recv8)

clientSocketSSL.send('QUIT\r\n'.encode())
recv9 = clientSocketSSL.recv(1024).decode()
print(recv9)
if recv9.split('\n')[1][:3] != '221':
    print('221 reply not received from server.')

clientSocketSSL.close()