/*
 * MQTT Publisher using ESP8266 AT Instruction Set
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
 * for STM32 F4DISC1 ST Core
 * ESP8266----------STM32F103
 * TX     ----------RX(PD9)
 * RX     ----------TX(PD8)
 * 
 * for STM32 DIYMROE_F407VGT/BLACK_F407VE/BLACK_F407VG ST Core
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
#define STOP_BUTTON     2 // 0: Disable STOP_BUTTON
#define RUNNING_LED     3 // 0: Disable RUNNING_LED
#define _BAUDRATE_      4800
#define _SERIAL_        Serial2
#define _MODEL_         "ATmega328"

//for Arduino MEGA(ATmega2560)
#elif defined(__AVR_ATmega2560__)
#define STOP_BUTTON     2 // 0: Disable STOP_BUTTON
#define RUNNING_LED     3 // 0: Disable RUNNING_LED
#define _BAUDRATE_      115200
#define _SERIAL_        Serial1
#define _MODEL_         "ATmega2560"

//for STM32F103(MAPLE Core)
#elif defined(__STM32F1__)
#define STOP_BUTTON     PB2 // 0: Disable STOP_BUTTON
#define RUNNING_LED     PB1 // 0: Disable RUNNING_LED
#define _BAUDRATE_      115200
#define _SERIAL_        Serial2
#define _MODEL_         "STM32F103 MAPLE Core"

//for STM32F103(ST Core)
#elif defined(ARDUINO_BLUEPILL_F103C8) || defined(ARDUINO_BLACKPILL_F103C8) || defined(ARDUINO_MAPLEMINI_F103CB)
HardwareSerial Serial2(PA3, PA2);
#define STOP_BUTTON     PB11 // 0: Disable STOP_BUTTON
#define RUNNING_LED     PB10 // 0: Disable RUNNING_LED
#define _BAUDRATE_      115200
#define _SERIAL_        Serial2
#define _MODEL_         "STM32F103 ST Core"

//for STM32F4DISC1(ST Core)
#elif defined(ARDUINO_DISCO_F407VG)
HardwareSerial Serial3(PD9, PD8);
#define STOP_BUTTON     PB11 // 0: Disable STOP_BUTTON
#define RUNNING_LED     PB10 // 0: Disable RUNNING_LED
#define _BAUDRATE_      115200
#define _SERIAL_        Serial3
#define _MODEL_         "STM32 F4DISC1 ST Core"

//for STM32F407(ST Core)
#elif defined(ARDUINO_DIYMROE_F407VGT) || defined(ARDUINO_BLACK_F407VE) || defined(ARDUINO_BLACK_F407VG)
HardwareSerial Serial2(PA3, PA2);
#define STOP_BUTTON     PB11 // 0: Disable STOP_BUTTON
#define RUNNING_LED     PB10 // 0: Disable RUNNING_LED
#define _BAUDRATE_      115200
#define _SERIAL_        Serial2
#define _MODEL_         "STM32F407 ST Core"

//for STM32 NUCLEO64(ST Core)
#else
HardwareSerial Serial1(PA10, PA9);
#define STOP_BUTTON     PB2 // 0: Disable STOP_BUTTON
#define RUNNING_LED     PB1 // 0: Disable RUNNING_LED
#define _BAUDRATE_      115200
#define _SERIAL_        Serial1
#define _MODEL_         "STM32 NUCLEO64 ST Core"
#endif


#define INTERVAL        100
#define MQTT_SERVER     "192.168.10.40"
//#define MQTT_SERVER     "broker.hivemq.com"
//#define MQTT_SERVER     "iot.eclipse.org"
#define MQTT_PORT       1883
#define MQTT_TOPIC      "ESP-AT-MQTT/"          // You can change
#define MQTT_KEEP_ALIVE 60
#define MQTT_WILL_TOPIC "ESP-AT-MQTT/"          // You can change
#define MQTT_WILL_MSG   "I am leaving..."       // You can change
#define _DEBUG_         0                       // for Debug

// Last Packet Send Time (MilliSecond)
unsigned long lastSendPacketTime = 0;

int buildConnect(char *buf, int keep_alive, char *client_id, char *will_topic, char *will_msg) {
  int rlen = 12;
  int pos = 14;

  int client_id_len = strlen(client_id);
  //Serial.println(client_id_len);
  buf[pos++] = 0x00;
  buf[pos++] = client_id_len;
  for(int i=0;i<client_id_len;i++) {
    buf[pos++] = client_id[i];
  }
  rlen = rlen + 2 + client_id_len;
  
  int will_topic_len = strlen(will_topic);
//  Serial.print("will_topic_len=");
//  Serial.println(will_topic_len);
  int will_msg_len = strlen(will_msg);
//  Serial.print("will_msg_len=");
//  Serial.println(will_msg_len);

  if (will_topic_len > 0 && will_msg_len > 0) {
    buf[pos++] = 0x00;
    buf[pos++] = will_topic_len;
    for(int i=0;i<will_topic_len;i++) {
      buf[pos++] = will_topic[i];
    }
    buf[pos++] = 0x00;
    buf[pos++] = will_msg_len;
    for(int i=0;i<will_msg_len;i++) {
      buf[pos++] = will_msg[i];
    }
    rlen = rlen + 2 + will_topic_len + 2 + will_msg_len;  
  }

  buf[0] = 0x10;
  buf[1] = rlen;
  buf[2] = 0x00;
  buf[3] = 0x06;
  buf[4] = 'M';
  buf[5] = 'Q';
  buf[6] = 'I';
  buf[7] = 's';
  buf[8] = 'd';
  buf[9] = 'p';
  buf[10] = 0x03;
  buf[11] = 0x02;
  if (will_topic_len > 0 && will_msg_len > 0) buf[11] = 0x06;
  buf[12] = 0x00;
  buf[13] = keep_alive;
  return buf[1] + 2;  
}

int buildPublish(char *buf, char *topic, char *msg) {
  int tlen = strlen(topic);
  for(int i=0;i<tlen;i++) {
    buf[4+i] = topic[i];
  }
  int mlen = strlen(msg);
  for(int i=0;i<mlen;i++) {
    buf[4+tlen+i] = msg[i];
  }
  buf[0] = 0x30;
  buf[1] = tlen + mlen + 2;
  buf[2] = 0x00;
  buf[3] = tlen;
  return buf[1] + 2;   
}

void mqttPingreq() {
  static long lastKeepAlive = 0;
  char pingreq[] = {0xc0,0x00};
  long now = millis();
  if (lastKeepAlive  > now) return;

  //Send to PINGREQ
  Serial.println();
  Serial.println("Sending PINGREQ");
  int ret = sendData(-1, pingreq, 2, "", 0);
  if (ret) errorDisplay("PINGREQ send fail");

  //Wait for PINGRESP
  waitForString("+IPD", 4, 1000);
#if 0
  if (!waitForString("+IPD", 4, 5000)) {
    errorDisplay("PINGRESP Fail");
  }
#endif
  getResponse(1000);
  //ATmegaではlong=int*1000はマイナスになる
  //long=long(int)*1000は正しい値になる
  lastKeepAlive = now + long(MQTT_KEEP_ALIVE) * 1000;
  if (lastKeepAlive < 0) lastKeepAlive = long(MQTT_KEEP_ALIVE) * 1000; // OverFlow
//  Serial.print("lastKeepAlive(9)=");
//  Serial.println(lastKeepAlive);
}

void mqttPublish(char * buf, int blen) {
  //Send to PUBLISH
  int ret = sendData(-1, buf, blen, "", 0);
  if (ret) errorDisplay("PUBLISH send fail");

  //Wait for PUBACK
  waitForString("+IPD", 4, 1000);
#if 0
  if (!waitForString("+IPD", 4, 5000)) {
    errorDisplay("PUBACK Fail");
  }
#endif
  getResponse(1000);
}


void mqttDisconnect() {
  char disconnect[] = {0xe0,0x00};
  //Send to DISCONNECT
  int ret = sendData(-1, disconnect, 2, "", 0);
  if (ret) errorDisplay("DISCONNECT send fail");

  //Wait for CLOSE
  if (!waitForString("CLOSE", 5, 5000)) {
    errorDisplay("CLOSE Fail");
  }
  clearBuffer();
}


void setup(){
  char cmd[128];
  int msize;

  Serial.begin(115200);
  delay(5000);
  Serial.print("_MODEL_=");
  Serial.println(_MODEL_);
  _SERIAL_.begin(_BAUDRATE_);

  //Save Serial Object
  serialSetup(_SERIAL_);

  if (RUNNING_LED) {
    pinMode(RUNNING_LED,OUTPUT);
    digitalWrite(RUNNING_LED,LOW);
  }
  if (STOP_BUTTON) pinMode(STOP_BUTTON, INPUT);   

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

  //Establishes TCP Connection
  sprintf(cmd,"AT+CIPSTART=\"TCP\",\"%s\",%d",MQTT_SERVER,MQTT_PORT);
  sendCommand(cmd);
  if (!waitForString("OK", 2, 5000)) {
    Serial.println("Check your MQTT SERVER"); 
    errorDisplay("AT+CIPSTART Fail");
  }
  clearBuffer();

  //Client requests a connection to a server
  Serial.print("MQTT CONNECT.....");
  //Client requests a connection to a server
  msize = buildConnect(cmd,MQTT_KEEP_ALIVE,MACaddress,MQTT_WILL_TOPIC,MQTT_WILL_MSG);
  if (_DEBUG_) hexDump(cmd,msize);
  int ret = sendData(-1, cmd, msize, "", 0);
  if (ret) errorDisplay("MQTT Connect Fail");

  //Wait for CONNACK
  if (!waitForString("+IPD", 4, 5000)) {
    errorDisplay("CONNACK Fail");
  }
  getResponse(1000);
  Serial.println("OK");

  Serial.println("Start MQTT Publish [" + String(_MODEL_) + "] to " + String(MQTT_SERVER));
  lastSendPacketTime = millis();
}

void loop(){
  static int counter = 0;
  static int timer = INTERVAL;
  static int ledStatus = 1;
  char msg[128];
  char payload[128];
  int msize;

  int buttonState = digitalRead(STOP_BUTTON);
  if (buttonState == 1) {
    mqttDisconnect();
    Serial.println();
    Serial.println("Sending DISCONNECT");
    Serial.println("Publish end");
    if (RUNNING_LED) digitalWrite(RUNNING_LED,LOW);
    while(1) { }
  }
  
  unsigned long now = millis();
  if ( (now - lastSendPacketTime) < 0) {
    lastSendPacketTime = now;
  }

  if ( (now - lastSendPacketTime) > 1000) {
    lastSendPacketTime = now;
    timer++;
    if (RUNNING_LED) digitalWrite(RUNNING_LED,ledStatus);
    ledStatus = !ledStatus;
#if 0
    Serial.print("ledStatus=");
    Serial.print(ledStatus);
    Serial.print("timer=");
    Serial.print(timer);
#endif
    if ( (timer % 10) == 0) Serial.print("+");
    if ( (timer % 10) != 0) Serial.print(".");

    if (timer >= INTERVAL) { // Publish
      //Publish message
      sprintf(msg,"Publish from %s #%03d",_MODEL_, counter);
      Serial.println();
      Serial.println("Sending PUBLISH");
      Serial.println(msg);
      counter++;
      if (counter == 1000) counter = 0;
      msize = buildPublish(payload, MQTT_TOPIC, msg);
      if (_DEBUG_) hexDump(payload,msize);
      mqttPublish(payload,msize);
      timer = 0;
    }
  } // endif
  mqttPingreq();
}
