# Arduino-ESPAT-TCP   
TCP/UDP Applicaton for Arduino using ESP8266's AT firmware.   
You don't need Ethernet card.   
You need only ESP8266 module.   

# Memory usage
__Saves Flash and SRAM__   

_UNO+ENC28J60+UIPEthernet+PubSubClient_

```
Sketch uses 25090 bytes (77%) of program storage space. Maximum is 32256 bytes.
Global variables use 1736 bytes (84%) of dynamic memory, leaving 312 bytes for local variables. Maximum is 2048 bytes.
Low memory available, stability problems may occur.
```

_UNO+W5100+Ethernet+PubSubClient_

```
Sketch uses 19582 bytes (60%) of program storage space. Maximum is 32256 bytes.
Global variables use 1101 bytes (53%) of dynamic memory, leaving 947 bytes for local variables. Maximum is 2048 bytes.
```

_UNO+ESP01+Software Serial+AT firmware_

```
Sketch uses 9938 bytes (30%) of program storage space. Maximum is 32256 bytes.
Global variables use 897 bytes (43%) of dynamic memory, leaving 1151 bytes for local variables. Maximum is 2048 bytes.
```

# Hardware requirements   
- ESP8266 module like ESP01   

- ATmega328/ATmega2560/STM32

# Software requirements   
- Arduino Environment   
 Core library is required when using STM32.   
 It supports Maple cores and ST cores.   

- ESP8266 AT Firmware   
 You can download from [here](https://github.com/espressif/ESP8266_NONOS_SDK/tags).   
 Some versions do not support 1M SoCs like ESP01.   

- Terminal software such as CoolTerm   
 I use TeraTrem.   
 You can down load from [here](https://download.cnet.com/tera-term/3000-2094_4-75766675.html).

# Applications   

**MQTT_Publish_ESP01**   
Simple MQTT Pubish Application.   

**MQTT_Subscribe_ESP01**   
Simple MQTT Subscribe Application.   

![slide1](https://user-images.githubusercontent.com/6020549/35101108-a13451d8-fca1-11e7-8cfd-37d71f18f880.JPG)

**TCP_Client_ESP01**   
Simple TCP Socket Client Application.   

**TCP_Server_ESP01**   
Simple TCP Socket Server Application.   

**UDP_Broadcast_Client_ESP01**   
Simple UDP Socket Client Application using UDP Broadcast.   

**UDP_Broadcast_Server_ESP01**   
Simple UDP Socket Server Application using UDP Broadcast.   


![slide2](https://user-images.githubusercontent.com/6020549/35101341-9019e394-fca2-11e7-9edd-0aa9086fd5db.JPG)

**NTP_Client_ESP01**   
Simple NTP Client Application.   
It require [this](https://github.com/PaulStoffregen/Time) library.   

**SNTP_Client_ESP01**   
Simple SNTP Client Application.   

![slide3](https://user-images.githubusercontent.com/6020549/35101499-241b1950-fca3-11e7-9876-0a22008ebc5a.JPG)

**SMTP_Client_gmail_ESP01**   
Simple SMTP Client Application.   
You need gmail account.   

![slide4](https://user-images.githubusercontent.com/6020549/35125598-90e2a360-fced-11e7-89ed-045cd6c49984.JPG)


# Flash AT firmware to ESP01.   
The 3.3V output of the UART-USB converter has too little current to be used.   
Power is supplied using 5V from the UART-USB converter and 3.3V from the regulator.   
- GPIO2 must be pulled up.   
- GPIO0 must be connected to GND.   
- CH_PD must be pulled up.   
- RESET must be pulled up.   

See [here](https://github.com/nopnop2002/Arduino-ESPAT-TCP/tree/master/Flash_AT_firmware) for details..   

![esp01-flash](https://user-images.githubusercontent.com/6020549/33159146-b8456238-d053-11e7-8202-a86cca2f8a3d.jpg)


---

# Setup ESP01 using terminal software such as CoolTerm.   
- GPIO2 must be pulled up.   
- GPIO0 must be pulled up.   
- CH_PD must be pulled up.   
- RESET must be pulled up.   

![esp01-setup](https://user-images.githubusercontent.com/6020549/33159150-bdade984-d053-11e7-9b93-bbbf05573441.jpg)

Connect to ESP01 at 115200 bps using terminal software.   

```
AT+GMR
AT version:1.6.2.0(Apr 13 2018 11:10:59)
SDK version:2.2.1(6ab97e9)
compile time:Jun  7 2018 19:34:26
Bin version(Wroom 02):1.6.2
OK

AT+CWMODE=1

OK
AT+CWLAP
+CWLAP:(3,"Picking",-86,"34:12:98:08:4b:4a",1,-4)
+CWLAP:(4,"ctc-g-fa4a2e",-92,"c0:25:a2:b1:8c:2e",2,3)
+CWLAP:(4,"aterm-e625c0-g",-49,"c0:25:a2:ac:cb:ba",3,15)
+CWLAP:(1,"aterm-e625c0-gw",-48,"c2:25:a2:ac:cb:ba",3,15)

OK

AT+CWJAP="Your AP's SSID","Your AP's password"
WIFI CONNECTED
WIFI GOT IP

OK

AT+CIPSTA?
+CIPSTA:ip:"192.168.10.142"
+CIPSTA:gateway:"192.168.10.1"
+CIPSTA:netmask:"255.255.255.0"

OK
AT+CWQAP

OK

WIFI DISCONNECT
```

_*** UNO ONLY ***_   
_*** Change baudrate to 4800bps ***_   
_*** Because there is no the 2nd UART in UNO ***_   
_*** So UNO use Software Serial with low speed ***_

```
AT+UART_DEF=4800,8,1,0,0
```

Re-Connect to ESP01 with 4800 bps.   

AT firmware has a function that automatically connects to the last connected AP when the module is reset.   
Using this function, you can omit the SSID and password.   

```
AT+RST
WIFI CONNECTED
WIFI GOT IP
AT+CIPSTA?
+CIPSTA:ip:"192.168.10.142"
+CIPSTA:gateway:"192.168.10.1"
+CIPSTA:netmask:"255.255.255.0"

OK
```

If you want to change the AP, execute the following command again.   
```
AT+CWJAP="New AP's SSID","New AP's password"
WIFI CONNECTED
WIFI GOT IP
```

----

# Connect ESP01 to UNO.

ESP01(Tx) - UNO(D4)   
ESP01(Rx) - UNO(D5)   

![ESP01-MQTT-UNO](https://user-images.githubusercontent.com/6020549/55268764-656f9f00-52d0-11e9-9120-360e397ffae0.jpg)

You can't use on-board 3.3V.    
An electric current is insufficient.   

----

# Connect ESP01 to MEGA2560.

ESP01(Tx) - MEGA(D19)   
ESP01(Rx) - MEGA(D18)   

![ESP01-MQTT-MEGA](https://user-images.githubusercontent.com/6020549/55268794-9fd93c00-52d0-11e9-8cca-4f4bd202d745.jpg)

You can't use on-board 3.3V.    
An electric current is insufficient.   

----

# Connect ESP01 to STM32F103(MAPLE Core).

ESP01(Tx) - STM32F103(PA3)   
ESP01(Rx) - STM32F103(PA2)   

![ESP01-MQTT-STM32F103_MAPLE-Core](https://user-images.githubusercontent.com/6020549/55272404-869bb400-52ff-11e9-97aa-1ff31090f925.jpg)

MAPLE Core.    
https://github.com/rogerclarkmelbourne/Arduino_STM32   

----

# Connect ESP01 to STM32 NUCLEO(ST Core).

ESP01(Tx) - STM32F103(PA10)   
ESP01(Rx) - STM32F103(PA9)   

![ESP01-MQTT-STM32F103_ST-Core](https://user-images.githubusercontent.com/6020549/55272409-94e9d000-52ff-11e9-9f4b-61386ed3e656.jpg)

ST Core.    
https://github.com/stm32duino/Arduino_Core_STM32   

----

# Connect ESP01 to STM32 F103 BluePill(ST Core).

ESP01(Tx) - STM32F103(PA3)   
ESP01(Rx) - STM32F103(PA2)   

![ESP01-MQTT-BLUEPILL_STM32F103_ST-Core](https://user-images.githubusercontent.com/6020549/62212468-c1ac1200-b3db-11e9-9fa2-9460d29b46cb.jpg)

Serial printing goes to PA9.   

----

# Connect ESP01 to STM32 F103 MapleMini(ST Core).

ESP01(Tx) - STM32F103(PA3)   
ESP01(Rx) - STM32F103(PA2)   

![ESP01-MQTT-MAPLEMINI_STM32F103_ST-Core](https://user-images.githubusercontent.com/6020549/62213727-88c16c80-b3de-11e9-9f10-54a274908c4c.jpg)

Serial printing goes to PA9.   

----

# STM32F103 development board on which ESP01 can be mounted(ST Core).

ESP01(Tx) - STM32F103(PA3)   
ESP01(Rx) - STM32F103(PA2)   

![ESP01-STM32](https://user-images.githubusercontent.com/6020549/103254648-7c718280-49c9-11eb-9bd0-c89cb686335e.JPG)

Serial printing goes to PA9.   

----

# Connect ESP01 to STM32 F303 BlackPill(ST Core).

ESP01(Tx) - STM32F303(PA3)   
ESP01(Rx) - STM32F303(PA2)   

Serial printing goes to PA9.   

----

# Connect ESP01 to STM32 F401 BlackPill(ST Core).

ESP01(Tx) - STM32F401(PA3)   
ESP01(Rx) - STM32F401(PA2)   

Serial printing goes to PA9.   

----

# Connect ESP01 to STM32 F4DISC1(ST Core).

ESP01(Tx) - STM32F4DISC1(PD9)   
ESP01(Rx) - STM32F4DISC1(PD8)   

I want to Fritzing Part of this board.   
Serial printing goes to PA2.   

----

# Connect ESP01 to STM32 F407 development board that like DIYMORE F407VGT.

ESP01(Tx) - STM32F407(PA3)   
ESP01(Rx) - STM32F407(PA2)   

https://stm32-base.org/boards/STM32F407VGT6-diymore   
Serial printing goes to PA9.   

----

# How to Firmware Upate

1.Make sure TE(terminal equipment) is in sta mode   
```
AT+CWMODE=1

OK
```

2.Make sure TE got ip address   
```
AT+CIPSTA?
+CIPSTA:ip:"192.168.10.115"
+CIPSTA:gateway:"192.168.10.1"
+CIPSTA:netmask:"255.255.255.0"

OK
```

3.Let's update   
```
AT+CIUPDATE
+CIPUPDATE:1    found server
+CIPUPDATE:2    connect server
+CIPUPDATE:3    got edition
+CIPUPDATE:4    start start

OK
```

4.Check firmware version   
```
AT+GMR
AT version:1.7.5.0(Oct 20 2021 19:14:04)
SDK version:3.0.5(b29dcd3)
compile time:Oct 20 2021 20:13:50
Bin version(Wroom 02):1.7.5
OK
```

----

# UART to WiFi module   
We can get a module for UART communication.   
We can write AT firmware to this.   
![ESP-UART-MODULE-1](https://user-images.githubusercontent.com/6020549/104827197-b504cd80-589e-11eb-95a8-f12c75670ced.JPG)

![ESP-UART-MODULE-2](https://user-images.githubusercontent.com/6020549/104827200-b8985480-589e-11eb-9a01-e70d4fbd55cc.JPG)

----

# ESP01 Adapter board
Only $1 for aliexpress/eBay.   
It has a 5V->3.3V regulator and can supply power directly from UNO's 5V.   
![ESP01-Adapter-3](https://user-images.githubusercontent.com/6020549/120063987-f3a86100-c0a4-11eb-95e4-c87164812af4.JPG)

----

# Use with TFT Shield   
UNO compatibles can have a pin header inside the pin socket.   
If you do not use SD-CARD, you can freely use digital pins # 10 to # 13 of the TFT shield.   
These pins can be used for UART communication with the ESP01.   

![ESP01-MQTT-UNO-TFT-2](https://user-images.githubusercontent.com/6020549/83363044-2277f280-a3d1-11ea-8688-76521819f1db.JPG)

![esp01-mqtt-uno-tft](https://user-images.githubusercontent.com/6020549/33193265-cbbd2618-d10a-11e7-9dba-dd60643c27bb.JPG)


