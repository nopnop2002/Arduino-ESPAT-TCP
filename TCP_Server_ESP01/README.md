# Environment
You need to change the following environment.

- #define LOCAL_IP        "192.168.10.21"   
Server IP Address   
- #define LOCAL_PORT      8080   
Server listen Port   


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

        client.settimeout(10.0)
        try:
            client.connect((args.host, args.port))
            #print("connect to host")
        except:
            print("connect fail")
            continue

        smsg = "data {} from client".format(counter)
        counter = counter + 1
        client.send(smsg.encode(encoding='utf-8'))
        print("[{}]---->".format(smsg),end="")

        client.settimeout(2.0)
        try:
            rmsg = client.recv(1024)
            if (type(rmsg) == bytes):
                rmsg=rmsg.decode('utf-8')
            print("[{}]".format(rmsg))
        except:
            print("receive timeout")

        client.close()
        time.sleep(2)
```
