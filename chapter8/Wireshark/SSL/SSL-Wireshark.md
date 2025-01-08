### 问题
#### A look at the captured trace
1. For each of the first 8 Ethernet frames, specify the source of the frame (client or server), determine the number of SSL records that are included in the frame, and list the SSL record types that are included in the frame. Draw a timing diagram between client and server, with one arrow for each SSL record.
2. Each of the SSL records begins with the same three fields (with possibly different values). One of these fields is “content type” and has length of one byte. List all three fields and their lengths.
3. Expand the ClientHello record. (If your trace contains multiple ClientHello records, expand the frame that contains the first one.) What is the value of the content type?
4. Does the ClientHello record contain a nonce (also known as a “challenge”)? If so, what is the value of the challenge in hexadecimal notation?
5. What (in hexadecimal notation) is the MAC BSS id on the beacon frame from 30 Munroe St?
6. Does the ClientHello record advertise the cyber suites it supports? If so, in the first listed suite, what are the public-key algorithm, the symmetric-key algorithm, and the hash algorithm?
7. Does this record include a nonce? If so, how long is it? What is the purpose of the client and server nonces in SSL?
8. Does this record include a session ID? What is the purpose of the session ID?
9. Does this record contain a certificate, or is the certificate included in a separate record. Does the certificate fit into a single Ethernet frame?
10. Locate the client key exchange record. Does this record contain a pre-master secret? What is this secret used for? Is the secret encrypted? If so, how? How long is the encrypted secret?
11. What is the purpose of the Change Cipher Spec record? How many bytes is the record in your trace?
12. In the encrypted handshake record, what is being encrypted? How?
13. Does the server also send a change cipher record and an encrypted handshake record to the client? How are those records different from those sent by the client?
14. How is the application data being encrypted? Do the records containing application data include a MAC? Does Wireshark distinguish between the encrypted application data and the MAC?
15. Comment on and explain anything else that you found interesting in the trace.

### 答案
1. 106/1/128.238.38.162/216.75.194.220/1/Client Hello
   108/2/216.75.194.220/128.238.38.162/1/Server Hello
   111/3/216.75.194.220/128.238.38.162/2/Certificate/Server Hello Done
   112/4/128.238.38.162/216.75.194.220/3/Client Key Exchange/Change Cipher Spec/Encrypted Handshake Message
   113/5/216.75.194.220/128.238.38.162/2/Change Cipher Spec/Encrypted Handshake Message
   114/6/128.238.38.162/216.75.194.220/1/Hypertext Transfer Protocol
   122/7/216.75.194.220/128.238.38.162/1/Hypertext Transfer Protocol
   149/8/216.75.194.220/128.238.38.162/1/Hypertext Transfer Protocol
2. Content Type:1/Version:2/Length:2
3. 22
4. 是,HEX是0x66df784c048cd60435dc448989469909
5. 具有,使用TLS规范,RSA公钥,RC4的128位对称密钥,MD5算法
6. 指定了,使用了第一个套件,TLS规范,RSA公钥,RC4的128位对称密钥,MD5算法
7. 包含random,有32字节长,主要防止攻击者回放攻击或重新排序
8. 包含一个32字节长的会话ID。目的是允许会话恢复,这可以显著减少耗时的服务器握手的次数来创建一个新的会话ID
9. 包含一个证书,长2687字节
10. 包含一个预主密钥。这个加密的预主密钥在服务器端被解密，并用于产生一个主密钥。然后利用这个主密钥生成“密钥块”,然后将其切成客户端MAC密钥、服务器MAC密钥、客户端加密密钥、服务器加密密钥、客户端IV和服务IV。该密钥将使用服务器的公钥进行加密,加密的密钥有132字节长。
11. 更改密码规范的目的是指示加密和身份验证算法的更改,并更新要在此连接上使用的密码套件。这个记录只有1字节
12. 此加密握手记录的发送方以及直到但不包括此消息的所有握手消息都将在记录中加密,这些信息使用两种哈希算法MD5和SHA进行连接和散列,此记录的内容是这两个哈希值的连接,已加密的握手记录用于验证密钥交换和身份验证过程是否成功
13. 会,唯一区别是发送方与接收方互换
14. 使用RSA,RC4,MD5进行加密。包含,不区分