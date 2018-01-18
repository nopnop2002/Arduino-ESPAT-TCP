/*
 * SMTP Client with AT firmware
 * 
 * ESP8266----------ATmega
 * TX     ----------RX(D4)
 * RX     ----------TX(D5)
 * 
 * ESP8266----------STM32F103
 * TX     ----------RX2(PA3)
 * RX     ----------TX2(PA2)
 * 
 */

#if defined(__AVR__)
#include <SoftwareSerial.h>
#define rxPin           4    // D4
#define txPin           5    // D5
SoftwareSerial Serial2(rxPin, txPin); // RX, TX
#define _MODEL_  "arduino"
#endif

#if defined(__STM32F1__)
#define _MODEL_  "stm32f103"
#endif

#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_PORT   465
#define BASE64_USER "Encorded Your Username of gmail" // Username of gmail
#define BASE64_PASS "Encorded Your Password of gmail" // Password of gmail
#define MAIL_FROM   "abcd@gmail.com"                  // Your gmail account
#define MAIL_TO     "abcd@provider.com"               // Mail To
#define JAPANESE    1                                 // Send Japanese
#define _DEBUG_     0

void putChar(char c) {
  char tmp[10];
  if ( c == 0x0a) {
    Serial.println();
  } else if (c == 0x0d) {
    
  } else if ( c < 0x20) {
    uint8_t cc = c;
    sprintf(tmp,"[0x%.2X]",cc);
    Serial.print(tmp);
  } else {
    Serial.print(c);
  }
}

//Wait for specific input string until timeout runs out
bool waitForString(char* input, int length, unsigned int timeout) {
  unsigned long end_time = millis() + timeout;
  char current_byte = 0;
  int index = 0;

   while (end_time >= millis()) {
    
      if(Serial2.available()) {
        
        //Read one byte from serial port
        current_byte = Serial2.read();
//        Serial.print(current_byte);
        if (_DEBUG_) putChar(current_byte);
        if (current_byte != -1) {
          //Search one character at a time
          if (current_byte == input[index]) {
            index++;
            
            //Found the string
            if (index == length) {              
              return true;
            }
          //Restart position of character to look for
          } else {
            index = 0;
          }
        }
      }
  }  
  //Timed out
  return false;
}

//Print error
void errorDisplay(char* buff) {
  Serial.print("Error:");
  Serial.println(buff);
  while(1) {}
}

void clearBuffer() {
  while (Serial2.available())
    Serial2.read();
//  Serial.println();
}

void getResponse(int timeout){
  char c;
  bool flag = false;
  char tmp[10];
  
  long int time = millis() + timeout;
  while( time > millis()) {
    if (Serial2.available()) {
      flag = true;
      c = Serial2.read();
      if (c == 0x0d) {
           
      } else if (c == 0x0a) {
        if (_DEBUG_) Serial.println();
      } else if ( c < 0x20) {
        uint8_t cc = c;
        sprintf(tmp,"[0x%.2X]",cc);
        if (_DEBUG_) Serial.print(tmp);
      } else {
        if (_DEBUG_) Serial.print(c);
      } 
    } // end if
  } // end while
  if (flag & _DEBUG_) Serial.println();
}


void hexDump(byte *buf, int msize) {
  Serial.print("\nmsize=");
  Serial.println(msize);
  for(int i=0;i<msize;i++) {
    if (buf[i] < 0x10) Serial.print("0");
    Serial.print(buf[i],HEX);
    Serial.print(" ");
  }
  Serial.println();
}

int sendSMTP(char *buf, int msize) {
  char at[128];
//  hexDump(buf,msize);
  sprintf(at,"AT+CIPSEND=%02d\r\n",msize);
  Serial2.print(at);
  if (!waitForString("OK", 2, 5000)) {
    errorDisplay("AT+CIPSEND Fail");
  }
  if (!waitForString(">", 1, 5000)) {
    errorDisplay("Server Not Response");
  }

  if (_DEBUG_) {
    Serial.println();
    Serial.print(">>>>>>");
    Serial.print(buf);
  }
  for (int i=0;i<msize;i++)Serial2.write(buf[i]);                  
  if (!waitForString("SEND OK", 7, 5000)) {
    errorDisplay("Server Not Receive my data");
  }

  return 1;
}

void setup(){
  char buf[128];
  int msize;

  delay(1000);
  Serial.begin(9600);

  //Make sure ESP8266 is set to 4800
  Serial2.begin(4800);

  Serial.println("Start SMTP Client [" + String(_MODEL_) + "] to " + String(SMTP_SERVER) );

  //Enable autoconnect
  Serial2.print("AT+RST\r\n");
  if (!waitForString("WIFI GOT IP", 11, 10000)) {
    errorDisplay("AT+RST Fail");
  }
  clearBuffer();

  //Set the size of SSL buffer
  Serial2.print("AT+CIPSSLSIZE=4096\r\n");
  if (!waitForString("OK", 2, 5000)) {
    errorDisplay("AT+CIPSSLSIZET Fail");
  }
  clearBuffer();

  //Establishes SSL Connection
  Serial.print("Connect " + String(SMTP_SERVER) + ".....");
  sprintf(buf,"AT+CIPSTART=\"SSL\",\"%s\",%d\r\n",SMTP_SERVER,SMTP_PORT);
  Serial2.print(buf);
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
