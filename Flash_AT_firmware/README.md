# How to flash AT firmware using Flash Download Tools

ESP8266 AT-Firmware writing uses the tools and firmware published below:   

- Flash Download Tools   
You can download from [here](http://www.espressif.com/en/support/download/other-tools).   

- AT-Firmware   
You can download from [here](https://github.com/espressif/ESP8266_NONOS_SDK/tags).   
The latest firmware may not support ESP01.   
So we flash version 2.2.1 and do an firmware update.   

# Goto UART Download Mode   
Start Flash Download Tools   
Select firmware and select com port.   
Connect the GPIO2 of the ESP8266 to GND to reset it.   
Start flash.   
![FLASH_DOWNLOAD_TOOLS-1](https://user-images.githubusercontent.com/6020549/233518393-50a92b0c-91ae-463e-8221-bc92657aedd7.jpg)   
![FLASH_DOWNLOAD_TOOLS-2](https://github.com/nopnop2002/Arduino-ESPAT-TCP/assets/6020549/cce795be-6bc3-4bed-827e-aeba213eeeb7)   
![FLASH_DOWNLOAD_TOOLS-3](https://github.com/nopnop2002/Arduino-ESPAT-TCP/assets/6020549/02cc6fe0-3d1c-44f0-aacc-8bbc46966d21)   
![FLASH_DOWNLOAD_TOOLS-4](https://github.com/nopnop2002/Arduino-ESPAT-TCP/assets/6020549/759aafbf-5e57-4dc5-beda-6a4c5abe7560)   

# Goto Flash Boot Mode   
Open terminal software such as CoolTerm and connect to ESP8266.   
The default baud rate is 115200bps.   
ESP8266's AT firmware treats CR+LF as the end of the command.   
Change the terminal software's transmission termination character to CR+LF.   
```AT<cr><lf>```   
Change GPIO2 of ESP8266 to PullUp and reset.   
![FLASH_DOWNLOAD_TOOLS-6](https://user-images.githubusercontent.com/6020549/233526247-7dcd8f4c-f8fd-4e48-a8f3-8a142e85cb0d.jpg)
