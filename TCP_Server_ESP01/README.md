# Environment
You need to change the following environment.

- #define MY_IP           "192.168.10.21"   
My IP Address   
- #define MY_PORT         "8080"   
Listen Port   


# TCP client using python
```
#!/usr/bin/python3
#-*- encoding: utf-8 -*-
import socket
import signal
import time
import argparse

def handler(signal, frame):
    global running
    print('handler')
    running = False

if __name__=='__main__':
    signal.signal(signal.SIGINT, handler)
    running = True

    parser = argparse.ArgumentParser()
    parser.add_argument('--host', help='tcp host', default="192.168.10.21")
    parser.add_argument('--port', type=int, help='tcp port', default=8080)
    args = parser.parse_args()
    print("host={}".format(args.host))
    print("port={}".format(args.port))

    counter = 0
    while running:
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print("create socket")

        client.connect((args.host, args.port))
        print("connect to host")

        msg = "data {} from client".format(counter)
        counter = counter + 1
        client.send(msg.encode(encoding='utf-8'))

        response = client.recv(1024)
        print("response={}".format(response))
        client.close()
        time.sleep(2)
```
