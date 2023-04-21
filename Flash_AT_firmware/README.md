# How to flash AT firmware using Flash Download Tools

ESP8266 AT-Firmware writing uses the tools and firmware published below:   

- Flash Download Tools   
You can download from [here](http://www.espressif.com/en/support/download/other-tools).   

- AT-Firmware   
You can download from [here](https://github.com/espressif/ESP8266_NONOS_SDK/tags).   
The latest firmware may not support ESP01.   
So we flash version 2.0 and do an firmware update.   

# Goto UART Download Mode   
Connect the GPIO2 of the ESP8266 to GND to reset it.   
![FLASH_DOWNLOAD_TOOLS-1](https://user-images.githubusercontent.com/6020549/233518393-50a92b0c-91ae-463e-8221-bc92657aedd7.jpg)
![FLASH_DOWNLOAD_TOOLS-2](https://user-images.githubusercontent.com/6020549/233518381-06a574b7-5ff5-4b0d-b0a6-15a0a7d1256f.jpg)
![FLASH_DOWNLOAD_TOOLS-3](https://user-images.githubusercontent.com/6020549/233518384-f3b4a556-dc46-4b00-ac69-f801c67d40a0.jpg)
![FLASH_DOWNLOAD_TOOLS-4](https://user-images.githubusercontent.com/6020549/233518389-cbecb778-7d5e-4a61-aca6-f5d7386350bd.jpg)

# Goto Flash Boot Mode   
Open terminal software such as CoolTerm and connect to ESP8266.   
The default baud rate is 115200bps.   
Connect the GPIO2 of the ESP8266 to PullUp to reset it.   
![FLASH_DOWNLOAD_TOOLS-6](https://user-images.githubusercontent.com/6020549/233526247-7dcd8f4c-f8fd-4e48-a8f3-8a142e85cb0d.jpg)
