# How to flash AT firmware using Flash Download Tools

ESP8266 AT-Firmware writing uses the tools and firmware published below:   

- Flash Download Tools   
You can download from [here](http://www.espressif.com/en/support/download/other-tools).   

- AT-Firmware   
You can download from [here](https://github.com/espressif/ESP8266_NONOS_SDK/tags).   
Some firmware version does not support ESP01.   
So we flash version 3.0.4 and do an firmware update.   
The latest version is 3.0.5.   
```
AT+GMR
AT version:1.7.5.0(Oct 20 2021 19:14:04)
SDK version:3.0.5(b29dcd3)
compile time:Oct 20 2021 20:13:50
Bin version(Wroom 02):1.7.5
OK
```

# Goto UART Download Mode   
Start Flash Download Tools   
Select firmware and select com port.   
Connect the GPIO2 of the ESP8266 to GND to reset it.   
Start flash.   
![FLASH_DOWNLOAD_TOOLS-1](https://user-images.githubusercontent.com/6020549/233518393-50a92b0c-91ae-463e-8221-bc92657aedd7.jpg)   
![FLASH_DOWNLOAD_TOOLS-2](https://github.com/nopnop2002/Arduino-ESPAT-TCP/assets/6020549/c79f0980-5021-4846-a46a-8a6ddcea6d23)
![FLASH_DOWNLOAD_TOOLS-3](https://github.com/nopnop2002/Arduino-ESPAT-TCP/assets/6020549/6e3c7bce-96ce-4481-80c7-8f44bf51f242)
![FLASH_DOWNLOAD_TOOLS-4](https://github.com/nopnop2002/Arduino-ESPAT-TCP/assets/6020549/7a0e28ef-bbdf-4d0e-ac6b-ac96860aaae8)

# Goto Flash Boot Mode   
Open terminal software such as CoolTerm and connect to ESP8266.   
The default baud rate is 115200bps.   
ESP8266's AT firmware treats CR+LF as the end of the command.   
Change the terminal software's transmission termination character to CR+LF.   
```AT<cr><lf>```   
Change GPIO2 of ESP8266 to PullUp and reset.   
![FLASH_DOWNLOAD_TOOLS-6](https://github.com/nopnop2002/Arduino-ESPAT-TCP/assets/6020549/ec7535d3-dfc5-4033-a113-980bcf2fd391)
