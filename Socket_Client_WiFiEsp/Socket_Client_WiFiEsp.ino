/*
 * Simple Socket Client with WiFiEsp
 * 
 * Software requirement
 * Arduino WiFi library for ESP8266 modules
 * https://github.com/bportaluri/WiFiEsp
 * 
 * ESP8266----------ATmega
 * TX     ----------RX(D4)
 * RX     ----------TX(D5)
 * 
 * ESP8266----------STM32F103
 * Not Supported
 *
*/

#include "WiFiEsp.h"  // https://github.com/bportaluri/WiFiEsp

#include "SoftwareSerial.h"
SoftwareSerial Serial1(4, 5); // RX, TX
#define _MODEL_        "WiFiEsp"
#define INTERVAL       5000
#define SOCKET_HOST    "192.168.10.190" // You have to change
#define SOCKET_PORT    9876             // You have to change
#define TIME_OUT       10000

WiFiEspClient client;

char ssid[] = "aterm-e625c0-g";       // your network SSID (name)
char pass[] = "05ecd1dcd39c6";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

unsigned long nextMillis;

//Print error
void errorDisplay(char* buff) {
  Serial.print("Error:");
  Serial.println(buff);
  while(1) {}
}

void setup() {
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  Serial1.begin(4800);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  
  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  nextMillis = millis();
  Serial.println("Start Socket Client [" + String(_MODEL_) + "] via ESP8266");
}

void loop() {
  static int num = 0;
  char smsg[30];
  char rmsg[30];
  int rflag;
  unsigned long timeout;
  unsigned long now;

  now = millis();
  if ( long(now - nextMillis) > 0) {
    nextMillis = millis() + INTERVAL;
    Serial.print("Client connect....");
    if (!client.connect(SOCKET_HOST, SOCKET_PORT)) {
      Serial.println("failed");
    } else {
      Serial.println("ok");
//      sprintf(smsg,"data from arduino %05d",num);
      sprintf(smsg,"data from %s %05d",_MODEL_, num);
      num++;
      client.write(smsg, strlen(smsg));

      // wait for responce
      rflag = 1;
      timeout = millis();
      while(client.available() == 0) {
        now = millis();
//        Serial.println("now="+String(now));
//        Serial.println("timeout="+String(timeout));
        if (long(now - timeout) > TIME_OUT) {
          rflag = 0;
        }
      } // end while

      Serial.print("Server response....");
      if (rflag == 0) {
        Serial.println("failed");
      } else {
        Serial.println("ok");
        int size;
        while((size = client.available()) > 0) {
//          Serial.print("["+String(MODULE)+" Socket Client]Receive Size=");
//          Serial.println(size);
          size = client.read(rmsg,size);
          Serial.write(smsg,size);
          Serial.write("->");
          Serial.write(rmsg,size);
          Serial.println("");
        } // end while
      }

      //disconnect client
      Serial.println("Client disconnect");
      client.stop();
    } // end if
  } // end if
}
