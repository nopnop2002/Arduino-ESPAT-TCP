/*
 * Simple Socket Server with AT Command
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

#define MY_IP       "192.168.10.190"
#define MY_PORT     "9876"
#define INTERVAL    5000
#define _DEBUG_  0

//answer strings from ESP
char str_buffer[64];
//command strings to ESP
char cmd[64];

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
bool waitForString(char* input, uint8_t length, unsigned int timeout) {

  unsigned long end_time = millis() + timeout;
//  int current_byte = 0;
  char current_byte = 0;
  uint8_t index = 0;

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
uint8_t readResponse(int id, char * buf,int sz_buf, int timeout) {
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

// Wait for Connect/Close
// irq=1 : Wait for CONNECT
// irq=2 : Wait for CLOSED
int waitConnect(int irq, int timeout) {
  int len=0;
  long int time = millis();
  char current_byte = 0;
  bool endFlag = 0;
  
  while( (time+timeout) > millis()) {
    while(Serial2.available())  {
      char current_byte = Serial2.read(); // read the next character.
      if (_DEBUG_) Serial.print(current_byte);

      if (current_byte == 0x0d) { // CR
          
      } else if (current_byte == 0x0a) { // LF
        if (_DEBUG_) {
          Serial.print("len=");
          Serial.println(len);
        }
        if (len == 0) continue;
        if (_DEBUG_) {
          Serial.print("str_buffer=[");
          Serial.print(str_buffer);
          Serial.println("]");
        }
        for(int i=0;i<len;i++) {
          if (str_buffer[i] == 0x2c) {
            if (irq == 1) {
              if (strcmp(&str_buffer[i+1],"CONNECT") != 0) return -1; // Not Connect
              str_buffer[i]=0;
              return atoi(str_buffer); // Connect ID
            }
            if (irq == 2) {
            if (strcmp(&str_buffer[i+1],"CLOSED") != 0) return -1; // Not Closed
              str_buffer[i]=0;
              return atoi(str_buffer); // Connect ID
            }
          } // end if
        } // end for

      } else {
        str_buffer[len++]=current_byte;
        if (len == 64) return -1;
        str_buffer[len]=0;
      } // end if
    } // end while 
  } // end while

  return -1;
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

  Serial.println("Start Socket Server [" + String(_MODEL_) + "] waiting " + String(MY_PORT) + " Port");
}

void loop(void) {
  char smsg[64];
  char rmsg[64];
  short rlen;
  short sz_smsg;
  int id;

  //Wait CONNECT
  id = waitConnect(1, 10000);
  if (_DEBUG_) {
    Serial.print("Connect id=");
    Serial.println(id);
  }
  if (id >= 0) {
    //Read data
    rlen = readResponse(id, rmsg, sizeof(rmsg), 5000);
    clearBuffer();
    if (_DEBUG_) {
      Serial.print("rmsg=[");
      Serial.print(rmsg);
      Serial.println("]");
    }
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
    sz_smsg=strlen(smsg);
    Serial.write((uint8_t *)smsg,sz_smsg);
    Serial.println();

    //Send AT+CIPSEND=id,nn
    sprintf(cmd,"AT+CIPSEND=%d,%d",id,sz_smsg);
    sendCommand(cmd);
    if (!waitForString(">", 1, 1000)) {
      errorDisplay("AT+CIPSEND Fail");
    } else {
      clearBuffer();
      //Send data
      sendCommand(smsg);
      waitForString("OK", 2, 1000);
      clearBuffer();
    }

    //wait for client disconnect
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
