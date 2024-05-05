UDP Broadcast Client using ESP8266 AT Instruction Set   

# Environment
You need to change the following environment.

- #define REMOTE_PORT     8080   
Port number of the UDP server to use   
- #define INTERVAL        5000   
Interval of Packet Send(MillSecond)   

# UDP server using python
```
#!/usr/bin/python3
# -*- coding : UTF-8 -*-
import select
import socket
import signal
import argparse

def handler(signal, frame):
    global running
    print('handler')
    running = False

if __name__=='__main__':
    signal.signal(signal.SIGINT, handler)
    running = True

    parser = argparse.ArgumentParser()
    parser.add_argument('--port', type=int, help='tcp port', default=8080)
    args = parser.parse_args()
    print("port={}".format(args.port))

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(('<broadcast>', args.port))
    sock.setblocking(0)

    while running:
        result = select.select([sock],[],[])
        msg = result[0][0].recv(1024)
        if (type(msg) == bytes):
            msg=msg.decode('utf-8')
        print(msg.strip())
```
