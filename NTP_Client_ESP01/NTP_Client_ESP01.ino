/*
 * NTP Client with AT Command
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
#include <TimeLib.h>     // https://github.com/PaulStoffregen/Time
#define rxPin    4    // D4
#define txPin    5    // D5
SoftwareSerial Serial2(rxPin, txPin); // RX, TX
#define _MODEL_  "arduino"
#endif

#if defined(__STM32F1__)
#include <TimeLib.h>     // https://github.com/PaulStoffregen/Time
#define _MODEL_  "stm32f103"
#endif

#define INTERVAL        60                     // Interval of Packet Send(Second)
#define NTP_SERVER      "ntp.jst.mfeed.ad.jp"  // NTP Server
#define NTP_PORT        123                    // NTP Port
#define LOCAL_PORT      2390                   // Local Port
#define LINK_ID         3                      // Link ID
#define NTP_PACKET_SIZE 48                     // NTP Packet Size
#define TIME_ZONE       9                      // Time Difference from GMT
#define _DEBUG_         0                      // for Debug

char cmd[64];

// NTP Packet Buffer (Send & Receive)
char packetBuffer[NTP_PACKET_SIZE];
// Last Packet Send Time (MilliSecond)
unsigned long lastSendPacketTime = 0;


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
      if (_DEBUG_) {
        Serial.print(current_byte,HEX);
        Serial.print("[");
        Serial.print(current_byte);
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

// dow_char() Return day of the week string(English) [Sun,Mon....]
char * dow_char_EN(byte days) {
  char *you[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
  return you[days];
}

// dow_char() Return day of the week string(Japanese) [日曜,火曜....]
char * dow_char_JP(byte days) {
  char *you[] = {"日曜","月曜","火曜","水曜","木曜","金曜","土曜"};
  return you[days];
}

// dow() Return day of the week number[0-Sunday, 1-Monday etc.]
byte dow(int y, int m, int d) {
  static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
//  y -= m < 3;
  if (m < 3) y--;
  return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

void showTime(char * title, time_t timet, char * dow) {
   Serial.print(title);
   Serial.print(year(timet));
   Serial.print("/");
   Serial.print(month(timet));
   Serial.print("/");
   Serial.print(day(timet));
   Serial.print(" ");
   Serial.print(hour(timet));
   Serial.print(":");
   Serial.print(minute(timet));
   Serial.print(":");
   Serial.print(second(timet));
   Serial.print(" [");
   Serial.print(dow);
   Serial.println("]");
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

  //Enable multi connections
  sendCommand("AT+CIPMUX=1");
  if (!waitForString("OK", 2, 1000)) {
    errorDisplay("AT+CIPMUX Fail");
  }
  clearBuffer();

  //Establish UDP Transmission
  //AT+CIPSTART=<link ID>,<type="UDP">,<remoteIP="0">,<remote port=0>,<UDP local port>,<UDP mode=2>
  sprintf(cmd,"AT+CIPSTART=%d,\"UDP\",\"0\",0,%u,2", LINK_ID, LOCAL_PORT);
//  sendCommand("AT+CIPSTART=3,\"UDP\",\"0\",0,2390,2");
  sendCommand(cmd);
  if (!waitForString("OK", 2, 1000)) {
    errorDisplay("AT+CIPMUX Fail");
  }
  clearBuffer();

  Serial.println("Start NTP Client [" + String(_MODEL_) + "] via ESP8266");

  // send an NTP packet to a time server
  sendNTPpacket(NTP_SERVER, NTP_PORT);
  lastSendPacketTime = millis();
}

void loop(void) {
  static int counter=0;
  byte DayOfWeek;
  
  long now = millis();
  if (now - lastSendPacketTime > 1000) { // One second has elapsed
    lastSendPacketTime = now;
    counter++;
    if ( (counter % 10) == 0) {
      Serial.print("+");
    } else {
      Serial.print(".");
    }
    if (_DEBUG_) Serial.println("counter=" + String(counter));
    if (counter == INTERVAL) {
      // Send request o NTP server
      sendNTPpacket(NTP_SERVER, NTP_PORT);
      counter=0;
    }
  }

  // Read data from NTP server
  int readLen = readResponse(3, packetBuffer, NTP_PACKET_SIZE, 1000);
//  Serial.print("packet received, length=");
//  Serial.println(readLen);
  if (readLen == 48) {
    Serial.println("\npacket received, length=48");

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);

    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears; 
    Serial.print("Unix time = ");
    Serial.println(epoch);

    // Get day of the week
    byte DayOfWeek=dow(year(),month(),day());
    // Greenwich Mean Time(GMT)
    showTime("The UTC time is ", epoch, dow_char_EN(DayOfWeek));
    // Local time(Japan)
    showTime("Local time is ", epoch + (TIME_ZONE * 60 * 60), dow_char_JP(DayOfWeek));
  }
}

// send an NTP request to the time server at the given address
void sendNTPpacket(char *ntpSrv, int ntpPort)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)

  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  //Send Data
  //AT+CIPSEND=<link ID>,<length>,<remoteIP>,<remote port>
  sprintf(cmd,"AT+CIPSEND=%d,%u,\"%s\",%u", LINK_ID, NTP_PACKET_SIZE, ntpSrv, ntpPort);
  sendCommand(cmd);
  if (!waitForString(">", 1, 1000)) {
    errorDisplay("AT+CIPSEND Fail");
  }
  clearBuffer();
  
  //Send Packet
  Serial2.write((uint8_t *)packetBuffer,NTP_PACKET_SIZE);
  if (!waitForString("SEND OK", 7, 1000)) {
     errorDisplay("AT+CIPSEND Fail");
  }
}

