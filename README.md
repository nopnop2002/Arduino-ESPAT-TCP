# MQTT_via_ESP01   
TCP Applicaton from UNO/NANO/ProMini/STM32F103 using ESP8266's AT firmware.   

**MQTT_Publish_ESP01**   
Simple Pubish Application.   
It's supprt arduino & stm32f103.   
It's NOT need any library.   

**MQTT_Subscribe_ESP01**   
Simple Subscribe Application.   
It's supprt arduino & stm32f103.   
It's NOT need any library.   

![slide1](https://user-images.githubusercontent.com/6020549/35101108-a13451d8-fca1-11e7-8cfd-37d71f18f880.JPG)

**Socket_Client_ESP01**   
Simple Socket Client Application.   
It's supprt arduino & stm32f103.   
It's NOT need any library.   

**Socket_Server_ESP01**   
Simple Socket Server Application.   
It's supprt arduino & stm32f103.   
It's NOT need any library.   

**Socket_Client_WiFiEsp**   
Socket Client Application using WiFiEsp.   
It's supprt arduino ONLY.   
It's need Arduino WiFi library for ESP8266 modules.   

**Socket_Server_WiFiEsp**   
Socket Server Application using WiFiEsp.   
It's supprt arduino ONLY.   
It's need Arduino WiFi library for ESP8266 modules.   

# Flash AT firmware to ESP-01.   

![esp01-flash](https://user-images.githubusercontent.com/6020549/33159146-b8456238-d053-11e7-8202-a86cca2f8a3d.jpg)

I'm using esp8266_flasher.exe and v2.0 AT Firmware(ESP).bin.   

---

# Setup ESP-01 using terminal software such as CoolTerm.   

![esp01-setup](https://user-images.githubusercontent.com/6020549/33159150-bdade984-d053-11e7-9b93-bbbf05573441.jpg)

    AT+GMR
    AT version:0.40.0.0(Aug  8 2015 14:45:58)
    SDK version:1.3.0
    Ai-Thinker Technology Co.,Ltd.
    Build:1.3.0.2 Sep 11 2015 11:48:04
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
    
    AT+UART_DEF=4800,8,1,0,0


_*** change baudrate to 4800bps ***_



    OK
    at
    
    OK
    AT+RST

    OK
    
    Ai-Thinker Technology Co.,Ltd.
    
    invalid
    WIFI CONNECTED
    WIFI GOT IP

----

# Connect ESP-01 to UNO.

ESP-01(Tx) - Arduino(D4)   
ESP-01(Rx) - Arduino(D5)   

![esp01-mqtt-uno](https://user-images.githubusercontent.com/6020549/33831441-78b95a54-debb-11e7-9001-059a0b1a1876.jpg)

You can't use on-board 3.3V.    
An electric current is insufficient.   

----

# Connect ESP-01 to STM32F101.

ESP-01(Tx) - STM32F103(PA3)   
ESP-01(Rx) - STM32F103(PA2)   

![esp01-mqtt-stm32f103](https://user-images.githubusercontent.com/6020549/33991405-d2e906de-e111-11e7-8691-d7a58492d3b8.jpg)

I'm using STM32 Core.    
https://github.com/rogerclarkmelbourne/Arduino_STM32   

----

# Software requirement
Arduino WiFi library for ESP8266 modules   
https://github.com/bportaluri/WiFiEsp   

----

# Flash sketch to your UNO/STM32F103.

Enjoy!!   

![esp01-mqtt-uno-tft](https://user-images.githubusercontent.com/6020549/33193265-cbbd2618-d10a-11e7-9dba-dd60643c27bb.JPG)

