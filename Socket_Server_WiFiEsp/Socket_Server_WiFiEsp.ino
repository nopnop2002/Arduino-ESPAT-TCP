/*
 * Simple Socket Server with WiFiEsp
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
 */

#include "WiFiEsp.h"  // https://github.com/bportaluri/WiFiEsp

#include "SoftwareSerial.h"
SoftwareSerial Serial2(4, 5); // RX, TX
#define _MODEL_        "WiFiEsp"

#define SOCKET_PORT    9876             // You have to change

WiFiEspServer server(SOCKET_PORT);

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
  Serial2.begin(4800);
  // initialize ESP module
  WiFi.init(&Serial2);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  IPAddress myIP(192,168,10,190);
  WiFi.config(myIP);

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

  server.begin();
  Serial.println("Start Socket Server [" + String(_MODEL_) + "] via ESP8266");
}

void loop()
{
  size_t size;
  WiFiEspClient client;
  
  if (client = server.available()) {
    while((size = client.available()) > 0) {
//      size = client.available();
//      Serial.print("["+String(MODULE)+" Socket Server]Receive size=");
//      Serial.println(size);
      char* rmsg = (char*)malloc(size);
      char* smsg = (char*)malloc(size);
      memset(smsg,0,sizeof(smsg));
      size = client.read(rmsg,size);
      for (int i=0; i< size; i++){
        if(isalpha(rmsg[i])) {
          smsg[i] = toupper(rmsg[i]);
        } else {
          smsg[i] = rmsg[i];
        }
      } // end for
      Serial.write(rmsg,size);
      Serial.write("->");
      Serial.write(smsg,size);
      Serial.println("");

      free(rmsg);
      client.write(smsg,size);
      free(smsg);
    } // end while

    //wait for client disconnect
    while(1) {
      int r = client.connected();
//      Serial.println("r=" + String(r));
      if (r == 0) break;
    }
    Serial.println("client disconnect");
    client.stop();
  }
}

