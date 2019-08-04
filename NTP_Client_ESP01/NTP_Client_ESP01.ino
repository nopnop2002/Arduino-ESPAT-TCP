/*
 * NTP Client using ESP8266 AT Instruction Set
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
#include <TimeLib.h>     // https://github.com/PaulStoffregen/Time

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

#define INTERVAL        10                     // Interval of Packet Send(Second)
#define NTP_SERVER      "ntp.jst.mfeed.ad.jp"  // NTP Server
#define NTP_PORT        123                    // NTP Port
#define LOCAL_PORT      2390                   // Local Port
#define LINK_ID         3                      // Link ID
#define NTP_PACKET_SIZE 48                     // NTP Packet Size
#define TIME_ZONE       9                      // Time Difference from GMT


// NTP Packet Buffer (Send & Receive)
char packetBuffer[NTP_PACKET_SIZE];
// Last Packet Send Time (MilliSecond)
unsigned long lastSendPacketTime = 0;

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
uint8_t dow(unsigned long t) {
  return ((t / 86400) + 4) % 7;
}

void showTime(char * title, time_t timet, char * dow) {
  //Serial.println("[showTime] timet=" + String(timet));
  Serial.print("[showTime] timet=");
  Serial.println(timet);
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

  //Enable multi connections
  sendCommand("AT+CIPMUX=1");
  if (!waitForString("OK", 2, 1000)) {
    errorDisplay("AT+CIPMUX Fail");
  }
  clearBuffer();

  //Establish UDP Transmission
  //AT+CIPSTART=<link ID>,<type="UDP">,<remoteIP="0">,<remote port=0>,<UDP local port>,<UDP mode=2>
  char cmd[64];
  sprintf(cmd,"AT+CIPSTART=%d,\"UDP\",\"0\",0,%u,2", LINK_ID, LOCAL_PORT);
//  sendCommand("AT+CIPSTART=3,\"UDP\",\"0\",0,2390,2");
  sendCommand(cmd);
  if (!waitForString("OK", 2, 1000)) {
    errorDisplay("AT+CIPSTART Fail");
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
//    if (_DEBUG_) Serial.println("counter=" + String(counter));
    if (counter == INTERVAL) {
      // Send request o NTP server
      sendNTPpacket(NTP_SERVER, NTP_PORT);
      counter=0;
    }
  }

  // Read data from NTP server
  int readLen = readResponse(LINK_ID, packetBuffer, NTP_PACKET_SIZE, 1000);
//  Serial.print("packet received, length=");
//  Serial.println(readLen);
  if (readLen == NTP_PACKET_SIZE) {
    Serial.println("\nNTP packet received");

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

    // Greenwich Mean Time(GMT)
    uint8_t DayOfWeek = dow(epoch); 
    showTime("The UTC time is ", epoch, dow_char_EN(DayOfWeek));

    // Local time(JAPAN)
    if (TIME_ZONE != 0) {
      DayOfWeek = dow(epoch + (TIME_ZONE * 60 * 60));
      showTime("Local time is ", epoch + (TIME_ZONE * 60 * 60), dow_char_JP(DayOfWeek));
    }
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
  int ret = sendData(LINK_ID, packetBuffer, NTP_PACKET_SIZE, ntpSrv, ntpPort);
  if (ret) {
     errorDisplay("sendData Fail");
  }

}
