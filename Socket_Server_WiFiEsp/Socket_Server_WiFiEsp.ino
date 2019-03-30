/*
 * Simple TCP/IP Server using WiFiEsp
 *  
 * Software requirement
 * Arduino WiFi library for ESP8266 modules
 * https://github.com/bportaluri/WiFiEsp
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
 */

#include "WiFiEsp.h"  // https://github.com/bportaluri/WiFiEsp

//for Arduino UNO(ATmega328)
#if defined(__AVR_ATmega328__)  || defined(__AVR_ATmega328P__)
#include "SoftwareSerial.h"
SoftwareSerial Serial1(4, 5); // RX, TX
#define _MODEL_         "ATmega328"
#define _BAUDRATE_      4800

//for Arduino MEGA(ATmega2560)
#elif defined(__AVR_ATmega2560__)
#define _MODEL_         "ATmega2560"
#define _BAUDRATE_      115200
#endif

#define SOCKET_PORT    9876             // You have to change

WiFiEspServer server(SOCKET_PORT);

char ssid[] = "aterm-e625c0-g";       // your network SSID (name)
char pass[] = "05ecd1dcd39c6";        // your network password
int status = WL_IDLE_STATUS;          // the Wifi radio's status

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
  Serial1.begin(_BAUDRATE_);
  // initialize ESP module
  WiFi.init(&Serial1);

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

