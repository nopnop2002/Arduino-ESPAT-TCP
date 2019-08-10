/*
 * Simple TCP/IP Server using ESP8266 AT Instruction Set
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
 * for STM32 F103 BLUEPILL/BLACKPILL/MAPLEMINI ST Core
 * ESP8266----------STM32
 * TX     ----------RX(PA3)
 * RX     ----------TX(PA2)
 * 
 * for STM32 F303 BLACKPILL ST Core
 * ESP8266----------STM32
 * TX     ----------RX(PA3)
 * RX     ----------TX(PA2)
 * 
 * for STM32 F4DISC1 ST Core
 * ESP8266----------STM32
 * TX     ----------RX(PD9)
 * RX     ----------TX(PD8) 
 *   
 * for STM32 DIYMROE_F407VGT/BLACK_F407VE/BLACK_F407VG ST Core
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
SoftwareSerial Serial2(SERIAL_RX, SERIAL_TX); // RX, 
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

//for STM32F4DISC1(ST Core)
#elif defined(ARDUINO_DISCO_F407VG)
HardwareSerial Serial3(PD9, PD8);
#define _BAUDRATE_      115200
#define _SERIAL_        Serial3
#define _MODEL_         "STM32 F4DISC1 ST Core"

//for STM32F407(ST Core)
#elif defined(ARDUINO_DIYMROE_F407VGT) || defined(ARDUINO_BLACK_F407VE) || defined(ARDUINO_BLACK_F407VG)
HardwareSerial Serial2(PA3, PA2);
#define _BAUDRATE_      115200
#define _SERIAL_        Serial2
#define _MODEL_         "STM32F407 ST Core"

//for STM32 NUCLEO64(ST Core)
#else
HardwareSerial Serial1(PA10, PA9);
#define _BAUDRATE_      115200
#define _SERIAL_        Serial1
#define _MODEL_         "STM32 NUCLEO64 ST Core"
#endif

#define MY_IP           "192.168.10.190"       // MY IP
#define MY_PORT         "9876"                 // Listen Port

//command to ESP
char cmd[64];

void setup(void)
{
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

  //Set IP address of Station
  sprintf(cmd, "AT+CIPSTA_CUR=\"%s\"", MY_IP);
  sendCommand(cmd);
  if (!waitForString("OK", 2, 1000)) {
    errorDisplay("AT+CIPSTA_CUR fail");
  }
  clearBuffer();

  //Get My IP Address
  char IPaddress[64];
  getIpAddress(IPaddress,sizeof(IPaddress),2000);
  Serial.print("IP Address: ");
  Serial.println(IPaddress);

  //Enable multi connections
  sendCommand("AT+CIPMUX=1");
  if (!waitForString("OK", 2, 1000)) {
    errorDisplay("AT+CIPMUX Fail");
  }
  clearBuffer();

  //Configure as TCP server 
  sprintf(cmd, "AT+CIPSERVER=1,%s", MY_PORT);
  sendCommand(cmd);
  if (!waitForString("OK", 2, 1000)) {
    errorDisplay("AT+CIPSERVER Fail");
  }
  clearBuffer();

  Serial.println("Start TCP/IP Server [" + String(_MODEL_) + "] wait for " + String(MY_PORT) + " Port");
}

void loop(void) {
  char smsg[64];
  char rmsg[64];
  int rlen;
  int id;

  //Wait from client connection
  id = waitConnect(1, 10000);
  if (_DEBUG_) {
    Serial.print("Connect id=");
    Serial.println(id);
  }
  if (id >= 0) {
    //Receive data
    rlen = readResponse(id, rmsg, sizeof(rmsg), 5000);
    clearBuffer();
    Serial.write((uint8_t *)rmsg,rlen);
    Serial.print("----->");
    memset (smsg,0,sizeof(smsg));
    for (int i=0; i< rlen; i++) {
      if(isalpha(rmsg[i])) {
        smsg[i] = toupper(rmsg[i]);
      } else {
        smsg[i] = rmsg[i];
      }
    }
    Serial.write((uint8_t *)smsg,rlen);
    Serial.println();

    //Send response
    int ret = sendData(id, smsg, rlen, "", 0);
    if (ret) {
       errorDisplay("sendData Fail");
    }
    
    //Wait from client disconnection
    while(1) {
      id = waitConnect(2, 10000);
      if (_DEBUG_) {
        Serial.print("Close id=");
        Serial.println(id);
      }
      if (id >= 0) break;
    }
  
  }
}
