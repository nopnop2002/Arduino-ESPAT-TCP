/*
 * SMTP Client using ESP8266 AT Instruction Set
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


#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_PORT   465
#define BASE64_USER "Base64 Encorded Your Username of gmail" // Username of gmail
#define BASE64_PASS "Base64 Encorded Your Password of gmail" // Password of gmail
#define MAIL_FROM   "mailFrom@gmail.com"                     // Your gmail account
#define MAIL_TO     "mailTo@provider.com"                    // Mail To
#define JAPANESE    0                                        // Japanese mail contents


void setup(){
  char buf[128];
  int msize;

  Serial.begin(115200);
  delay(5000);
  Serial.print("_MODEL_=");
  Serial.println(_MODEL_);
  _SERIAL_.begin(_BAUDRATE_);

  //Save Serial Object
  serialSetup(_SERIAL_);

  Serial.println("Start SMTP Client [" + String(_MODEL_) + "] to " + String(SMTP_SERVER) );

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

  //Set the size of SSL buffer
  sendCommand("AT+CIPSSLSIZE=4096");
  if (!waitForString("OK", 2, 5000)) {
    errorDisplay("AT+CIPSSLSIZET Fail");
  }
  clearBuffer();

  //Establishes SSL Connection
  Serial.print("Connect " + String(SMTP_SERVER) + ".....");
  sprintf(buf,"AT+CIPSTART=\"SSL\",\"%s\",%d\r\n",SMTP_SERVER, SMTP_PORT);
  sendCommand(buf);
  if (!waitForString("220 smtp.gmail.com", 18, 5000)) {
    errorDisplay("AT+CIPSTART Fail");
  }
  getResponse(2000);
  Serial.println("OK");

  //Sending EHLO
  Serial.print("Send EHLO.....");
  sprintf(buf,"EHLO www.example.com\r\n");
  msize = strlen(buf);
  sendSMTP(buf, msize);
  if (!waitForString("250 SMTPUTF8", 12, 1000)) {
    errorDisplay("EHLO Fail");
  }
  getResponse(1000);
  Serial.println("OK");

  //Sending AUTH LOGIN
  Serial.print("Send AUTH LOGIN.....");
  sprintf(buf,"AUTH LOGIN\r\n");
  msize = strlen(buf);
  sendSMTP(buf, msize);
  if (!waitForString("334", 3, 1000)) {
    errorDisplay("AUTH LOGIN Fail");
  }
  getResponse(1000);
  Serial.println("OK");

  //Sending User Name of Bas64 Encording
  Serial.print("Send USER.....");
  sprintf(buf,"%s\r\n",BASE64_USER);
  msize = strlen(buf);
  sendSMTP(buf, msize);
  if (!waitForString("334", 3, 1000)) {
    errorDisplay("USER Fail");
  }
  getResponse(1000);
  Serial.println("OK");

  //Sending Password of Bas64 Encording
  Serial.print("Send PASSWORD.....");
  sprintf(buf,"%s\r\n",BASE64_PASS);
  msize = strlen(buf);
  sendSMTP(buf, msize);
  if (!waitForString("235", 3, 1000)) {
    errorDisplay("PASSWORD FROM Fail");
  }
  getResponse(1000);
  Serial.println("OK");

  //Sending MAIL FROM
  Serial.print("Send MAIL FROM.....");
  sprintf(buf,"MAIL FROM: <%s>\r\n",MAIL_FROM);
  msize = strlen(buf);
  sendSMTP(buf, msize);
  if (!waitForString("250", 3, 1000)) {
    errorDisplay("MAIL FROM Fail");
  }
  getResponse(1000);
  Serial.println("OK");

  //Sending RCPT TO
  Serial.print("Send RCPT TO.....");
  sprintf(buf,"RCPT TO: <%s>\r\n",MAIL_TO);
  msize = strlen(buf);
  sendSMTP(buf, msize);
  if (!waitForString("250", 3, 1000)) {
    errorDisplay("RCPT TO Fail");
  }
  getResponse(1000);
  Serial.println("OK");

  //Sending DATA
  Serial.print("Send DATA.....");
  sprintf(buf,"DATA\r\n");
  msize = strlen(buf);
  sendSMTP(buf, msize);
  if (!waitForString("354", 3, 1000)) {
    errorDisplay("DATA Fail");
  }
  getResponse(1000);
  Serial.println("OK");

  //Sending From
  sprintf(buf,"From: <%s>\r\n",MAIL_FROM);
  msize = strlen(buf);
  getResponse(1000);

  //Sending To
  sprintf(buf,"To: <%s>\r\n",MAIL_TO);
  msize = strlen(buf);
  sendSMTP(buf, msize);
  getResponse(1000);

  //Sending Subject
  sprintf(buf,"Subject: Mail From %s\r\n\r\n",_MODEL_);
  msize = strlen(buf);
  sendSMTP(buf, msize);
  getResponse(1000);

  //Sending Message
  sprintf(buf,"Hello\r\n");
  msize = strlen(buf);
  sendSMTP(buf, msize);
  getResponse(1000);

  sprintf(buf,"This mail from %s + ESP-01\r\n",_MODEL_);
  msize = strlen(buf);
  sendSMTP(buf, msize);
  getResponse(1000);

  if (JAPANESE) {
  sprintf(buf,"このメールは%s + ESP-01からのメールです\r\n",_MODEL_);
  msize = strlen(buf);
  sendSMTP(buf, msize);
  getResponse(1000);
  }
  
  //Sending .
  Serial.print("Send End of Data.....");
  sprintf(buf,".\r\n");
  msize = strlen(buf);
  sendSMTP(buf, msize);
  if (!waitForString("250", 3, 1000)) {
    errorDisplay("End of Data Fail");
  }
  getResponse(1000);
  Serial.println("OK");

  //Sending QUIT
  Serial.print("QUIT.....");
  sprintf(buf,"QUIT\r\n");
  msize = strlen(buf);
  sendSMTP(buf, msize);
  if (!waitForString("221", 3, 1000)) {
    errorDisplay("QUIT Fail");
  }
  Serial.println("OK");

  waitForString("CLOSE", 5, 1000);
  getResponse(1000);
  Serial.println("Mail send!!");
}

void loop(){

}
