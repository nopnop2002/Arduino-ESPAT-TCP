/*
 * Simple TCP/IP Client using ESP8266 AT Instruction Set
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
 * for STM32F103 MAPLE Core
 * ESP8266----------STM32F103
 * TX     ----------RX(PA3)
 * RX     ----------TX(PA2)
 * 
 * for STM32 BLUEPILL/BLACKPILL/MAPLEMINI ST Core
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
#elif defined(ARDUINO_BLUEPILL_F103C8) || defined(ARDUINO_BLUEPILL_F103C8) || defined(ARDUINO_MAPLEMINI_F103CB)
HardwareSerial Serial2(PA3, PA2);
#define _BAUDRATE_      115200
#define _SERIAL_        Serial2
#define _MODEL_         "STM32F103 ST Core"

//for STM32 NUCLEO64(ST Core)
#else
HardwareSerial Serial1(PA10, PA9);
#define _BAUDRATE_      115200
#define _SERIAL_        Serial1
#define _MODEL_         "STM32 NUCLEO64 ST Core"
#endif

#define SERVER          "192.168.10.190"       // Server IP
#define PORT            9876                   // Server port
#define INTERVAL        5000                   // Interval of Packet Send(Second)

//command to ESP
char cmd[64];
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
  getIpAddress(IPaddress, sizeof(IPaddress), 2000);
  Serial.print("IPaddress=[");
  Serial.print(IPaddress);
  Serial.println("]");

  //Get My MAC Address
  char MACaddress[64];
  getMacAddress(MACaddress, sizeof(MACaddress), 2000);
  Serial.print("MACaddress=[");
  Serial.print(MACaddress);
  Serial.println("]");

  Serial.println("Start TCP/IP Client [" + String(_MODEL_) + "] to " + String(SERVER) + "/" + String(PORT) );
  lastSendPacketTime = millis();
}

void loop(void) {
  static int num = 0;
  char smsg[64];
  char rmsg[64];
  int sz_smsg;

  // If there is some input, a program is ended.
  if (Serial.available() > 0) {
    char inChar = Serial.read();
    Serial.println("KeyIn");
    sendCommand("AT+CWQAP");
    if (!waitForString("OK", 2, 1000)) {
      errorDisplay("AT+CWQAP Fail");
    }
    clearBuffer();
    Serial.println("client end");
    while (1) { }
  }

  long now = millis();
  if (( now - lastSendPacketTime) > 0) {
    lastSendPacketTime = millis() + INTERVAL;
    //Start connection
    //AT+CIPSTART=<type="TCP">,<remote IP>,<remote port>
    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%u", SERVER, PORT);
    sendCommand(cmd);
    if (!waitForString("OK", 2, 10000)) {
      Serial.println("Check Server IP Address and Server Port");
      Serial.print("Server IP:");
      Serial.println(SERVER);
      Serial.print("Server Port:");
      Serial.println(PORT);
      errorDisplay("AT+CIPSTART Fail");
    }
    clearBuffer();

    sprintf(smsg, "data from %s %05d", _MODEL_, num);
    sz_smsg = strlen(smsg);
    Serial.write((uint8_t *)smsg, sz_smsg);
    num++;

    //Send Data
    int ret = sendData(-1, smsg, sz_smsg, "", 0);
    if (ret) {
      errorDisplay("sendData Fail");
    }

    //Read Response
    int readLen = readResponse(-1, rmsg, sizeof(rmsg), 5000);
    if (_DEBUG_) {
      Serial.println();
      Serial.print("readLen=");
      Serial.println(readLen);
    }
    Serial.print("---->");
    Serial.write((uint8_t *)rmsg, readLen);
    Serial.println();
    if (readLen < 0) {
      errorDisplay("Server not response");
    }
    clearBuffer();

    //Close TCP connection
    sendCommand("AT+CIPCLOSE");
    if (!waitForString("OK", 2, 1000)) {
      errorDisplay("AT+CIPCLOSE Fail");
    }
    clearBuffer();
  }
}
