# Environment
You need to change the following environment.

- #define LOCAL_PORT      8080   
Server listen Port   


# UDP broadcast client using python
```
#!/usr/bin/python
#-*- encoding: utf-8 -*-
import socket
import signal
import time
import argparse


ADDRESS = "255.255.255.255" # limited broadcast address
#ADDRESS = "<broadcast>" # limited broadcast address

def handler(signal, frame):
    global running
    print('handler')
    running = False

if __name__=='__main__':
    signal.signal(signal.SIGINT, handler)
    running = True

    parser = argparse.ArgumentParser()
    parser.add_argument('--port', type=int, help='udp port', default=8080)
    args = parser.parse_args()
    print("port={}".format(args.port))

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    counter = 0
    while running:
        smsg = "data {} from client".format(counter)
        counter = counter + 1
        sock.sendto(smsg.encode('utf-8'), (ADDRESS, args.port))
        print("[{}]---->".format(smsg),end="")
        time.sleep(1)

        sock.settimeout(2.0)
        try:
            rmsg, cli_addr = sock.recvfrom(1024)
            if (type(rmsg) == bytes):
                rmsg=rmsg.decode('utf-8')
            print("[{}]".format(rmsg))
        except:
            print("receive timeout")
            continue

    sock.close()
```
