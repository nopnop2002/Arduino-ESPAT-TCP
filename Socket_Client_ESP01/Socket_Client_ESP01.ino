/*
 * Simple Socket Client with AT Command
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
#define rxPin    4    // D4
#define txPin    5    // D5
SoftwareSerial Serial2(rxPin, txPin); // RX, TX
#define _MODEL_  "arduino"
#endif

#if defined(__STM32F1__)
#define _MODEL_  "stm32f103"
#endif

#define SERVER      "192.168.10.190"
#define PORT        "9876"
#define INTERVAL    5000
#define _DEBUG_  0

char cmd[64];
unsigned long lastmillis;

int getIpAddress(char *buf, int szbuf, int timeout) {
  int len=0;
  int pos=0;
  char line[128];
    
  long int time = millis();

  Serial2.print("AT+CIPSTA?\r\n");

  while( (time+timeout) > millis()) {
    while(Serial2.available())  {
      char c = Serial2.read(); // read the next character.
      if (c == 0x0d) {
          
      } else if (c == 0x0a) {
        if (_DEBUG_) {
          Serial.print("Read=[");
          Serial.print(line);
          Serial.println("]");
        }
        int offset;
        for(offset=0;offset<pos;offset++) {
          if(line[offset] == '+') break;
        }
        if (strncmp(&line[offset],"+CIPSTA:ip:",11) == 0) {
          strcpy(buf,&line[12+offset]);
          len = strlen(buf) - 1;
          buf[len] = 0;
        }
        if (strcmp(line,"OK") == 0) return len;
        pos=0;
        line[pos]=0;
      } else {
        line[pos++]=c;
        line[pos]=0;
      }
    }  
  }
  return len;
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
        if (_DEBUG_) Serial.print(current_byte);
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

//Remove all bytes from the receive buffer
void clearBuffer() {
  while (Serial2.available())
    Serial2.read();
  if (_DEBUG_) Serial.println("");
}

//Send AT Command
void sendCommand(char* buff) {
  if (_DEBUG_) {
    Serial.println("");
    Serial.print(buff);
    Serial.println("-->");
  }
  Serial2.println(buff);
  Serial2.flush();
}

//Print error
void errorDisplay(char* buff) {
  Serial.print("Error:");
  Serial.println(buff);
  while(1) {}
}

//Receive Message
int readResponse(int id, char * buf,int sz_buf, int timeout) {
  int len=0;
  int flag=0;
  int datalen;
  long int time = millis();

  // id < 0  +IPD,nn:ReceiveData
  // id = 0  +IPD,0,nn:ReceiveData
  // id > 0  +IPD,id,nn:ReceiveData
  while( (time+timeout) > millis()) {
    while(Serial2.available())  {
      char current_byte = Serial2.read(); // read the next character.
      if (_DEBUG_) Serial.print(current_byte);
      if (flag == 0) {
        if (current_byte != 0x2c) continue;
        flag++;
        if (id < 0) flag++;
      } else if (flag == 1) {
        if (current_byte != 0x2c) continue;
        flag++;
      } else if (flag == 2) {
        if (current_byte == 0x3a) { // :
          datalen=atoi(buf);
          flag++;
          len=0;
        } else {
          buf[len++]=current_byte;
          buf[len]=0;
        }
      } else {
        buf[len++]=current_byte;
        if (len == sz_buf) return -len;
        buf[len]=0;
        if (len == datalen) return len;
      }
    } // end while 
  } // end while
  return -len;
}

void setup(void)
{
  Serial.begin(9600);

  //Make sure ESP8266 is set to 4800
  Serial2.begin(4800);
  delay(100);

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

  //Get local IP address 
  sendCommand("AT+CIPSTA?");
  if (!waitForString("OK", 2, 1000)) {
    errorDisplay("AT+CIPSTA? Fail");
  }
  clearBuffer();

  //Get My IP Address
  char IPaddress[64];
  getIpAddress(IPaddress,sizeof(IPaddress),2000);
  Serial.print("IP Address: ");
  Serial.println(IPaddress);

  lastmillis = millis();
  Serial.println("Start Socket Client [" + String(_MODEL_) + "] to " + String(SERVER) + "/" + String(PORT) );
}

void loop(void) {
  static int num = 0;
  char smsg[64];
  char rmsg[64];
  short sz_smsg;

  if (Serial.available() > 0) {
    char inChar = Serial.read();
    Serial.print("KeyIn");
    sendCommand("AT+CWQAP");
    if (!waitForString("OK", 2, 1000)) {
      errorDisplay("AT+CWQAP Fail");
    }
    clearBuffer();
    Serial.println("client end");
    while(1) { }
  }

  if (((signed long)(millis() - lastmillis)) > 0) {
    lastmillis = millis() + INTERVAL;
    //Start connection
    //Send AT+CIPSTART="TCP","192.168.XXX.XXX",PORT
    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", SERVER,PORT);
    sendCommand(cmd);
    if (!waitForString("OK", 2, 10000)) {
      errorDisplay("AT+CIPSTART Fail");
    }
    clearBuffer();
//    sprintf(smsg,"data from arduino %05d",num);
    sprintf(smsg,"data from %s %05d",_MODEL_, num);
    num++;
    sz_smsg=strlen(smsg);
    //Send AT+CIPSEND=nn
    sprintf(cmd,"AT+CIPSEND=%d",sz_smsg);
    sendCommand(cmd);
    if (!waitForString(">", 1, 1000)) {
      errorDisplay("AT+CIPSEND Fail");
    }
    clearBuffer();
    Serial.write((uint8_t *)smsg,sz_smsg);
    
    //Send data
    sendCommand(smsg);
    if (!waitForString("SEND OK", 7, 1000)) {
       errorDisplay("AT+CIPSEND Fail");
    }
    clearBuffer();
    //Read data
    int readLen = readResponse(-1, rmsg, sizeof(rmsg), 5000);
    if (_DEBUG_) {
      Serial.println();
      Serial.print("readLen=");
      Serial.println(readLen);
    }
    Serial.print("---->");
    Serial.write((uint8_t *)rmsg,readLen);
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
