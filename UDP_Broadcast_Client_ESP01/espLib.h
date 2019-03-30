/*
 * Socket library using ESP8266 AT Instruction Set
 */

#define _DEBUG_ 0

//Serial Object
Stream *espSerial = 0;

//Save Serial Object
void serialSetup(Stream &port) {
  espSerial = &port;
}

//Wait for specific input string until timeout runs out
bool waitForString(char* input, int length, unsigned int timeout) {

  unsigned long end_time = millis() + timeout;
  char current_byte = 0;
  int index = 0;

   while (end_time >= millis()) {
      if(espSerial->available()) {
        
        //Read one byte from serial port
        current_byte = espSerial->read();
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
  while (espSerial->available())
    espSerial->read();
  if (_DEBUG_) Serial.println("");
}

//Send AT Command
void sendCommand(char* buff) {
  if (_DEBUG_) {
    Serial.println("");
    Serial.print(buff);
    Serial.println("-->");
  }
  espSerial->println(buff);
  espSerial->flush();
}

//Send Message
int sendData(int id, char *buf, int blen, char *rmote, unsigned int port) {
  char atcmd[64];
  if (strlen(rmote)) {
    if (id >= 0) {
      //AT+CIPSEND=<link ID>,<length>,<remoteIP>,<remote port>
      sprintf(atcmd,"AT+CIPSEND=%d,%d,\"%s\",%u", id, blen, rmote, port);
    } else {
      //AT+CIPSEND=<length>,<remoteIP>,<remote port>
      sprintf(atcmd,"AT+CIPSEND=%d,\"%s\",%u", blen, rmote, port);
    }
  } else {
    if (id >= 0) {
      //AT+CIPSEND=<link ID>,<length>
      sprintf(atcmd,"AT+CIPSEND=%d,%d", id, blen);
    } else {
      //AT+CIPSEND=<length>
      sprintf(atcmd,"AT+CIPSEND=%d", blen);
    }
  }
  sendCommand(atcmd);
  if (!waitForString(">", 1, 1000)) return 1;
  clearBuffer();
  
  //Send Packet
  for(int i=0;i<blen;i++) espSerial->write(buf[i]);
  if (!waitForString("SEND OK", 7, 1000)) return 2;
//  clearBuffer();
  return 0;  
}

//Receive Message
int readResponse(int id, char *buf, int sz_buf, int timeout) {
  int len=0;
  int flag=0;
  int datalen;
  long int time = millis();

  // id < 0  +IPD,nn:ReceiveData
  // id = 0  +IPD,0,nn:ReceiveData
  // id > 0  +IPD,id,nn:ReceiveData
  while( (time+timeout) > millis()) {
    while(espSerial->available())  {
      char current_byte = espSerial->read(); // read the next character.
      if (_DEBUG_) {
        Serial.print("0x");
        if (current_byte < 0x10) Serial.print("0"); 
        Serial.print(current_byte,HEX);
        Serial.print("[");
        if (current_byte < 0x20) {
          Serial.print(" ");
        } else if (current_byte > 0x7F) {
          Serial.print(" ");
        } else {
          Serial.print(current_byte);
        }
        Serial.print("]");
        Serial.print(flag);
        Serial.print(" ");
        Serial.print(datalen);
        Serial.print(" ");
        Serial.println(len);
      }
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
        }
      } else {
        buf[len++]=current_byte;
        if (len == datalen) return len;
      }
    } // end while 
  } // end while
  return -len;
}

//Get IP Address
int getIpAddress(char *buf, int szbuf, int timeout) {
  int len=0;
  int pos=0;
  char line[128] = {0};
    
  long int time = millis();

  sendCommand("AT+CIPSTA?");

  while( (time+timeout) > millis()) {
    while(espSerial->available())  {
      char c = espSerial->read(); // read the next character.
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

//Get MAC Address
int getMacAddress(char *buf, int szbuf, int timeout) {
  int len=0;
  int pos=0;
  char line[128] = {0};
    
  long int time = millis();

  sendCommand("AT+CIPSTAMAC?");

  while( (time+timeout) > millis()) {
    while(espSerial->available())  {
      char c = espSerial->read(); // read the next character.
      if (c == 0x0d) {
          
      } else if (c == 0x0a) {
        if (_DEBUG_) {
          Serial.print("Read=[");
          Serial.print(line);
          Serial.println("]");
        }
        if (strncmp(line,"+CIPSTAMAC:",11) == 0) {
          strcpy(buf,&line[12]);
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

//Print error
void errorDisplay(char* buff) {
  Serial.print("Error:");
  Serial.println(buff);
  while(1) {}
}
