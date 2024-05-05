# Environment
You need to change the following environment.

- #define SERVER          "192.168.10.46"    
IP address of the TCP server to use   
- #define PORT            8080   
Port number of the TCP server to use   
- #define INTERVAL        5000   
Interval of Packet Send(MillSecond)   


# TCP server using python
```
#!/usr/bin/python3
# -*- coding : UTF-8 -*-
import socket
import argparse
server_ip = "0.0.0.0"

if __name__=='__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', type=int, help='tcp port', default=8080)
    args = parser.parse_args()
    print("port={}".format(args.port))

    listen_num = 5
    tcp_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_server.bind((server_ip, args.port))
    while True:
        tcp_server.listen(listen_num)

        client,address = tcp_server.accept()
        print("Connected!! [ Source : {}]".format(address))

        buffer_size = 1024
        while True:
            msg = client.recv(buffer_size)
            print("len(msg)={}".format(len(msg)))
            if (len(msg) == 0): break
            if (type(msg) == bytes):
                msg=msg.decode('utf-8')
            print("Received Data : {}".format(msg))
            client.send(b"OK")

        client.close()
```
