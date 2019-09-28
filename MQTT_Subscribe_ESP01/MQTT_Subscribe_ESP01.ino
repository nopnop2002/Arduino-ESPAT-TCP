/*
 * MQTT Subscriber using ESP8266 AT Instruction Set
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

//for STM32F303(ST Core)
#elif defined(ARDUINO_BLACKPILL_F303CC)
HardwareSerial Serial2(PA3, PA2);
#define STOP_BUTTON     PB11 // 0: Disable STOP_BUTTON
#define RUNNING_LED     PB10 // 0: Disable RUNNING_LED
#define _BAUDRATE_      115200
#define _SERIAL_        Serial2
#define _MODEL_         "STM32F303 ST Core"

//for STM32F401(ST Core)
#elif defined(ARDUINO_BLACKPILL_F401CC)
HardwareSerial Serial2(PA3, PA2);
#define STOP_BUTTON     PB13 // 0: Disable STOP_BUTTON
#define RUNNING_LED     PB12 // 0: Disable RUNNING_LED
#define _BAUDRATE_      115200
#define _SERIAL_        Serial2
#define _MODEL_         "STM32F401 ST Core"

//for STM32F4DISC1(ST Core)
#elif defined(ARDUINO_DISCO_F407VG)
HardwareSerial Serial3(PD9, PD8);
#define STOP_BUTTON     PB11 // 0: Disable STOP_BUTTON
#define RUNNING_LED     PB10 // 0: Disable RUNNING_LED
#define _BAUDRATE_      115200
#define _SERIAL_        Serial3
#define _MODEL_         "STM32 F4DISC1 ST Core"

//for STM32F407(ST Core)
#elif defined(ARDUINO_DIYMORE_F407VGT) || defined(ARDUINO_BLACK_F407VE) || defined(ARDUINO_BLACK_F407VG)
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

#define MQTT_SERVER     "192.168.10.40"         // You can change
//#define MQTT_SERVER     "broker.hivemq.com"
//#define MQTT_SERVER     "iot.eclipse.org"
#define MQTT_PORT       1883
#define SUB_TOPIC       "#"                    // You can change
#define MQTT_KEEP_ALIVE 60
#define MAX_TOPIC       64                     // You can change
#define MAX_PAYLOAD     64                     // You can change
#define _DEBUG_         0                      // for Debug

// Last Packet Send Time (MilliSecond)
unsigned long lastSendPacketTime = 0;

int buildConnect(char *buf, int keep_alive, char *client_id, char *will_topic, char *will_msg) {
  int rlen = 12;
  int pos = 14;

  int client_id_len = strlen(client_id);
//  Serial.println(client_id_len);
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

int buildSubscribe(char *buf, char *topic) {
  int tlen = strlen(topic);
  for(int i=0;i<tlen;i++) {
    buf[6+i] = topic[i];
  }
  buf[0] = 0x82;
  buf[1] = tlen + 5;
  buf[2] = 0x00;
  buf[3] = 0x01;
  buf[4] = 0x00;
  buf[5] = tlen;
  buf[tlen+6] = 0x00;
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

void putChar(char c) {
  char tmp[10];
  sprintf(tmp,"[0x%.2X]",c);
  Serial.print(tmp);
  Serial.print(" ");
  if ( c >= 0x20) {
    Serial.print("<");
    Serial.print(c);
    Serial.print(">");
  }
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
  Serial.print("MQTT CONNECTT.....");
  //ATmegaではNULL,NULLは正しく動かない
  msize = buildConnect(cmd,MQTT_KEEP_ALIVE,MACaddress,"","");
  if (_DEBUG_) hexDump(cmd,msize);
  int ret = sendData(-1, cmd, msize, "", 0);
  if (ret) errorDisplay("MQTT Connect Fail");

  //Wait for CONNACK
  if (!waitForString("+IPD", 4, 5000)) {
    errorDisplay("CONNACK Fail");
  }
  getResponse(1000);
  Serial.println("OK");

  //Client requests a subscribe to a server
  Serial.print("MQTT SUBSCRIBE.....");
  msize = buildSubscribe(cmd,SUB_TOPIC);
  if (_DEBUG_) hexDump(cmd,msize);
  ret = sendData(-1, cmd, msize, "", 0);
  if (ret) errorDisplay("MQTT Subscribe Fail");

  //Wait for SUBACK
  if (!waitForString("+IPD", 4, 5000)) {
    errorDisplay("SUBACK Fail");
  }
  getResponse(1000);
  Serial.println("OK");

  Serial.println("Start MQTT Subscribe [" + String(_MODEL_) + "] from " + String(MQTT_SERVER));
  lastSendPacketTime = millis();
}

void loop(){
  static int timer = 0;
  static int ledStatus = 1;

  int buttonState = digitalRead(STOP_BUTTON);
  if (buttonState == 1) {
    mqttDisconnect();
    Serial.println();
    Serial.println("Sending DISCONNECT");
    Serial.println("Subscribe end");
    if (RUNNING_LED) digitalWrite(RUNNING_LED,LOW);
    while(1) { }
  }

/*
 * Subscribe Message Structure
 * 
 * +IPD,AA:xCxEfffffffffggggg
 *         <------AA-------->
 * 
 * AA(ASCII):Total Data Length
 * C(BINARY):{number of f} ＋ {number of g} ＋ 2
 * E(BINARY):{number of f}
 * {number of g} = C-E-2
 * f:Topic
 * g:Payload
 * 
 * Example
 * +IPD,XX:[0x30][0x1E][0x00][0x18][24 byte Topic][4 byte Payload]
 * C=0x1E=30
 * E=0x18=24
 * {number of f}=0x18=24
 * {number of g}=30-24-2=4
 */
 

  int isSubscribe = 0;
  static int step = 0;
  static int len0 = 0;
  static int len1 = 0;
  static int len2 = 0;
  static int pos0 = 0;
  static int pos1 = 0;
  static int pos2 = 0;
  static char buf1[MAX_TOPIC];
  static char buf2[MAX_PAYLOAD];
  char puback[] = {0x40,0x02};
  char c;

  while (_SERIAL_.available()) {
    c = _SERIAL_.read();
    if (_DEBUG_) {
      putChar(c);
      Serial.print("step=");
      Serial.println(step);
    }
//    if (c == 0x0d) continue;
//    if (c == 0x0a) continue;
    if(step == 0 && c == '+') {
      step=1;
    } else if (step == 1) {
      if (c == 'I') step=2;
      else step=0;
    } else if (step == 2) {
      if (c == 'P') step=3;
      else step=0;
    } else if (step == 3) {
      if (c == 'D') step=4;
      else step=0;
    } else if (step == 4) {
      if (c == ',') step=5;
      else step=0;
    } else if (step == 5 && c == ':') {
      step=6;
      pos0 = 0;
      pos1 = 0;
      pos2 = 0;
    } else if (step == 6) { // get BCDE data
      if (pos0 == 1) len0 = c;    // len0 is C
      if (pos0 == 3) len1 = c;    // len1 is E
      pos0++;
      if (pos0 == 4) step=7;
    } else if (step == 7) { // get topic
      if (pos1 == MAX_TOPIC) {
        Serial.println("TOPIC is too large!!");
        Serial.println("Check MAX_TOPIC");
        continue;
      }
      buf1[pos1++]=c;
      buf1[pos1]=0;
//      Serial.print("pos1=");
//      Serial.println(pos1);
//      Serial.print("buf1=");
//      Serial.println(buf1);
      if (pos1 == len1) {
        step=8;
        len2 = len0 - len1 - 2; // len2 is payload size
        pos2 = 0;
      }
    } else if (step == 8) { // get payload
      if (pos2 == MAX_PAYLOAD) {
        Serial.println("PAYLOAD is too large!!");
        Serial.println("Check MAX_PAYLOAD");
        continue;
      }
      buf2[pos2++]=c;
      buf2[pos2]=0;
//      Serial.print("pos2=");
//      Serial.println(pos2);
//      Serial.print("buf2=");
//      Serial.println(buf2);
      if (pos2 == len2) {
        isSubscribe=1;
        step=0;
      }
#if 0
      Serial.print("step=");
      Serial.print(step);
      Serial.print(" len0=");
      Serial.print(len0);
      Serial.print(" len1=");
      Serial.print(len1);
      Serial.print(" len2=");
      Serial.print(len2);
      Serial.print(" pos1=");
      Serial.print(pos1);
      Serial.print(" pos2=");
      Serial.println(pos2);
#endif
    }
  }

  if (isSubscribe) {  //Publish acknowledgment
    Serial.println();
    Serial.println("Sending PUBACK");
    int ret = sendData(-1, puback, 2, "", 0);
    if (ret) errorDisplay("PUBACK send Fail");

    Serial.println();
    for(int i=0;i<40;i++) Serial.print("-");
    Serial.println();
    Serial.print("Topic=[");
    Serial.print(buf1);
    Serial.println("]");
    Serial.print("Payload=[");
    Serial.print(buf2);
    Serial.println("]");
    for(int i=0;i<40;i++) Serial.print("-");
    Serial.println();
    isSubscribe = 0;
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
    if ( (timer % 10) == 0) Serial.print("+");
    if ( (timer % 10) != 0) Serial.print(".");
  }
  mqttPingreq();
}
