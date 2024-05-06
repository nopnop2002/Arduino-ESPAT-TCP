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
import signal
import argparse
server_ip = "0.0.0.0"

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

    listen_num = 5
    socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    socket.bind((server_ip, args.port))

    while True:
        socket.listen(listen_num)
        socket.settimeout(1.0)
        try:
            client,address = socket.accept()
            print("Connected!! [ Source : {}]".format(address))
        except:
            #print("Timeout. running={}".format(running))
            if running is False: break
            continue

        while running:
            rmsg = client.recv(1024)
            print("len(rmsg)={}".format(len(rmsg)))
            if (len(rmsg) == 0): break
            if (type(rmsg) == bytes):
                rmsg=rmsg.decode('utf-8')
            print("[{}]".format(rmsg),end="")

            smsg = ""
            for ch in rmsg:
                #print("ch={}".format(ch))
                if ch.islower():
                    smsg = smsg + ch.upper()
                else:
                    smsg = smsg + ch.lower()

            print("---->[{}]".format(smsg))
            client.send(smsg.encode(encoding='utf-8'))

        client.close()
```
