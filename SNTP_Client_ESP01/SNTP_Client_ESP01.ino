/*
 * SNTP Client using ESP8266 AT Instruction Set
 *  
 * for ATmega328
 * ESP8266----------ATmega328
 * TX     ----------RX(D4)
 * RX     ----------TX(D5)
 * 
 * for ATmega2560
 * ESP8266----------ATmega2560
 * TX     ----------RX(D19)
 * RX     ----------TX(D18)
 * 
 * for STM32 F103 MAPLE Core
 * ESP8266----------STM32
 * TX     ----------RX(PA3)
 * RX     ----------TX(PA2)
 * 
 * for STM32 F103 ST Core
 * ESP8266----------STM32
 * TX     ----------RX(PA3)
 * RX     ----------TX(PA2)
 * 
 * for STM32 F303 BLACKPILL ST Core
 * ESP8266----------STM32
 * TX     ----------RX(PA3)
 * RX     ----------TX(PA2)
 * 
 * for STM32 F401 BLACKPILL ST Core
 * ESP8266----------STM32
 * TX     ----------RX(PA3)
 * RX     ----------TX(PA2)
 * 
 * for STM32 F4DISC1 ST Core
 * ESP8266----------STM32
 * TX     ----------RX(PD9)
 * RX     ----------TX(PD8)  
 *  
 * for STM32 F407VE/F407VG ST Core
 * ESP8266----------STM32
 * TX     ----------RX(PA3)
 * RX     ----------TX(PA2)
 *  
 * for STM32 NUCLEO64 ST Core
 * ESP8266----------STM32
 * TX     ----------RX(PA10)
 * RX     ----------TX(PA9)
 * 
 */

#include "espLib.h"

//for Arduino UNO(ATmega328)
#if defined(__AVR_ATmega328__)  || defined(__AVR_ATmega328P__)
#include <SoftwareSerial.h>
#define SERIAL_RX       4
#define SERIAL_TX       5
SoftwareSerial Serial2(SERIAL_RX, SERIAL_TX); // RX, TX
#define _BAUDRATE_      4800
#define _SERIAL_        Serial2
#define _MODEL_         "ATmega328"

//for Arduino MEGA(ATmega2560)
#elif defined(__AVR_ATmega2560__)
#define _BAUDRATE_      115200
#define _SERIAL_        Serial1
#define _MODEL_         "ATmega2560"

//for STM32F103(MAPLE Core)
#elif defined(__STM32F1__)
#define _BAUDRATE_      115200
#define _SERIAL_        Serial2
#define _MODEL_         "STM32F103 MAPLE Core"

//for STM32F103(ST Core)
#elif defined(ARDUINO_BLUEPILL_F103C8) || defined(ARDUINO_BLACKPILL_F103C8) || defined(ARDUINO_MAPLEMINI_F103CB)
HardwareSerial Serial2(PA3, PA2);
#define _BAUDRATE_      115200
#define _SERIAL_        Serial2
#define _MODEL_         "STM32F103 ST Core"

//for STM32F303(ST Core)
#elif defined(ARDUINO_BLACKPILL_F303CC)
HardwareSerial Serial2(PA3, PA2);
#define _BAUDRATE_      115200
#define _SERIAL_        Serial2
#define _MODEL_         "STM32F303 ST Core"

//for STM32F401(ST Core)
#elif defined(ARDUINO_BLACKPILL_F401CC)
HardwareSerial Serial2(PA3, PA2);
#define _BAUDRATE_      115200
#define _SERIAL_        Serial2
#define _MODEL_         "STM32F401 ST Core"

//for STM32F4DISC1(ST Core)
#elif defined(ARDUINO_DISCO_F407VG)
HardwareSerial Serial3(PD9, PD8);
#define _BAUDRATE_      115200
#define _SERIAL_        Serial3
#define _MODEL_         "STM32 F4DISC1 ST Core"

//for STM32F407(ST Core)
#elif defined(ARDUINO_DIYMORE_F407VGT) || defined(ARDUINO_BLACK_F407VE) || defined(ARDUINO_BLACK_F407VG)
HardwareSerial Serial2(PA3, PA2);
#define _BAUDRATE_      115200
#define _SERIAL_        Serial2
#define _MODEL_         "STM32F407 ST Core"

//for STM32 NUCLEO64(ST Core)#else
#else
HardwareSerial Serial1(PA10, PA9);
#define _BAUDRATE_      115200
#define _SERIAL_        Serial1
#define _MODEL_         "STM32 NUCLEO64 ST Core"
#endif

#define INTERVAL        10                     // Interval of Packet Send(Second)
#define SNTP_SERVER     "time.google.com"      // SNTP Server
#define TIME_ZONE       9                      // Time Difference from GMT
#define DNS_SERVER1     "8.8.8.8"              // DNS SERVER1
#define DNS_SERVER2     "8.8.4.4"              // DNS SERVER2


// Last Packet Send Time (MilliSecond)
unsigned long lastSendPacketTime = 0;

void setup(){
  Serial.begin(115200);
  delay(5000);
  Serial.print("_MODEL_=");
  Serial.println(_MODEL_);
  _SERIAL_.begin(_BAUDRATE_);

  //Save Serial Object
  serialSetup(_SERIAL_);

  //Enable autoconnect
  sendCommand("AT+CWAUTOCONN=1");
  if (!waitForString("OK", 2, 1000)) {
    errorDisplay("AT+CWAUTOCONN Fail");
  }
  clearBuffer();

  //Restarts the Module
  sendCommand("AT+RST");
  if (!waitForString("WIFI GOT IP", 11, 10000)) {
    errorDisplay("ATE+RST Fail");
  }
  clearBuffer();

  //Local echo off
  sendCommand("ATE0");
  if (!waitForString("OK", 2, 1000)) {
    errorDisplay("ATE0 Fail");
  }
  clearBuffer();

  //Get My IP Address
  char IPaddress[32];
  getIpAddress(IPaddress,sizeof(IPaddress),2000);
  Serial.print("IPaddress=[");
  Serial.print(IPaddress);
  Serial.println("]");

  //Get My MAC Address
  char MACaddress[32];
  getMacAddress(MACaddress,sizeof(MACaddress),2000);
  Serial.print("MACaddress=[");
  Serial.print(MACaddress);
  Serial.println("]");

  //Get DNS Server Information
  sendCommand("AT+CIPDNS_CUR?");
  if (!waitForString("OK", 2, 1000, true)) {
    errorDisplay("AT+CIPDNS_CUR Fail");
  }
  clearBuffer();

  //Set DNS Server Information
  //AT+CIPDNS_CUR=<enable>[,<DNS server0>,<DNS server1>]
  char cmd[128];
  sprintf(cmd,"AT+CIPDNS_CUR=1,\"%s\",\"%s\"", DNS_SERVER1, DNS_SERVER2);
  sendCommand(cmd);
  if (!waitForString("OK", 2, 1000)) {
    errorDisplay("AT+CIPDNS_CUR Fail");
  }
  clearBuffer();

  //Get DNS Server Information again
  sendCommand("AT+CIPDNS_CUR?");
  if (!waitForString("OK", 2, 1000, true)) {
    errorDisplay("AT+CIPDNS_CUR Fail");
  }
  clearBuffer();

  //Sets the Configuration of SNTP
#ifdef SNTP_SERVER
  sprintf(cmd,"AT+CIPSNTPCFG=1,%d,\"%s\"",TIME_ZONE,SNTP_SERVER);
#else
  sprintf(cmd,"AT+CIPSNTPCFG=1,%d",TIME_ZONE);
#endif
  sendCommand(cmd);
  if (!waitForString("OK", 2, 5000)) {
    errorDisplay("AT+CIPSNTPCFG Fail");
  }
  clearBuffer();

  Serial.println("Start SNTP Client [" + String(_MODEL_) + "] using " + String(SNTP_SERVER));
  lastSendPacketTime = millis();
}

void loop(){
  static int counter=0;
  char buf[128];

  long now = millis();
  if (now - lastSendPacketTime > 1000) { // One second has elapsed
    lastSendPacketTime = now;
    counter++;
    if ( (counter % 10) == 0) {
      Serial.print("+");
    } else {
      Serial.print(".");
    }
    if (counter == INTERVAL) {
      if (getSNTPtime(buf, 128, 5000)) {
        Serial.println();
        Serial.println("SNTP response is [" + String(buf) + "]");
      }
      counter = 0;
    }
    
  } // endif

}
