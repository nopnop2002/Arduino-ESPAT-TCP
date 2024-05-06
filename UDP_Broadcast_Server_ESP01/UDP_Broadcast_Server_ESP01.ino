/*
 * UDP Broadcast Client using ESP8266 AT Instruction Set
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
 * ESP8266----------STM32F103
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
 * ESP8266----------STM32F103
 * TX     ----------RX(PD9)
 * RX     ----------TX(PD8) 
 * 
 * for STM32 F407VE/F407VG ST Core 
 * ESP8266----------STM32F103
 * TX     ----------RX(PA3)
 * RX     ----------TX(PA2)  
 *  
 * for STM32 NUCLEO64 ST Core
 * ESP8266----------STM32F103
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

//for STM32 NUCLEO64(ST Core)
#else
HardwareSerial Serial1(PA10, PA9);
#define _BAUDRATE_      115200
#define _SERIAL_        Serial1
#define _MODEL_         "STM32 NUCLEO64 ST Core"
#endif

#define REMOTE_HOST     "255.255.255.255"      // Remote Host
#define REMOTE_PORT     28080                  // Remote Port
#define LOCAL_PORT      8080                   // Local Port
#define INTERVAL        1000                   // Interval of Packet Send(MillSecond)

// Last Packet Send Time (MilliSecond)
long lastSendPacketTime;

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

  //Get My IP Address
  char IPaddress[64];
  getIpAddress(IPaddress,sizeof(IPaddress),2000);
  Serial.print("IPaddress=[");
  Serial.print(IPaddress);
  Serial.println("]");
  
  //Get My MAC Address
  char MACaddress[64];
  getMacAddress(MACaddress,sizeof(MACaddress),2000);
  Serial.print("MACaddress=[");
  Serial.print(MACaddress);
  Serial.println("]");  

  //Establish UDP Transmission (Single connection)
  //AT+CIPSTART=<type="UDP">,<remote host>,<remote port>,<local port>,<mode=2>
  //mode=2
  //Each time UDP data is received, the <"remote host"> and <remote port> will be changed to the IP address and port of the device that sends the data.
  char cmd[64];
  sprintf(cmd,"AT+CIPSTART=\"UDP\",\"%s\",%u,%u,2", REMOTE_HOST, REMOTE_PORT, LOCAL_PORT);
  sendCommand(cmd);
  if (!waitForString("OK", 2, 1000)) {
    errorDisplay("AT+CIPSTART Fail");
  }
  clearBuffer();

  Serial.println("Start UDP Server [" + String(_MODEL_) + "] wait for " + String(LOCAL_PORT) + " Port");
}

void loop(void) {
  static int num = 0;
  char smsg[64];
  char rmsg[64];
  char stat[64];

  //If there is some input, a program is ended.
  if (Serial.available() > 0) {
    char inChar = Serial.read();
    Serial.println("KeyIn");
    //Close UDP connection
    sendCommand("AT+CIPCLOSE");
    if (!waitForString("OK", 2, 1000)) {
      errorDisplay("AT+CIPCLOSE Fail");
    }
    clearBuffer();
    //Disconnect from an AP
    sendCommand("AT+CWQAP");
    if (!waitForString("OK", 2, 1000)) {
      errorDisplay("AT+CWQAP Fail");
    }
    clearBuffer();
    Serial.println("client end");
    while (1) { }
  }

  //Read packet
  int rlen = readData(rmsg, sizeof(rmsg), 5000);
  if (_DEBUG_) {
    Serial.println();
    Serial.print("rlen=");
    Serial.println(rlen);
  }
  
  if (rlen > 0) {
    memset (smsg,0,sizeof(smsg));
    for (int i=0; i< rlen; i++) {
      if(isalpha(rmsg[i])) {
        smsg[i] = toupper(rmsg[i]);
      } else {
        smsg[i] = rmsg[i];
      }
    }
    Serial.write((uint8_t *)rmsg,rlen);
    Serial.print("----->");
    Serial.write((uint8_t *)smsg,rlen);
    Serial.println();
    
    //Send Data
    int ret = sendData(-1, smsg, rlen, "", 0);
    if (ret) {
      errorDisplay("sendData Fail");
    }
  }
}
