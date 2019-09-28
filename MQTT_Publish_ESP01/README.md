You can change these define.

MQTT server you use   
#define MQTT_SERVER     "192.168.10.40"   
//#define MQTT_SERVER     "broker.hivemq.com"   
//#define MQTT_SERVER     "iot.eclipse.org"   

MQTT Publish Interval(Second)   
#define INTERVAL        100

MQTT Publish Topic   
#define MQTT_TOPIC      "ESP-AT-MQTT/"

MQTT Will Topic   
#define MQTT_WILL_TOPIC "ESP-AT-MQTT/"

MQTT Will Payload   
#define MQTT_WILL_MSG   "I am leaving..."

Stop Button   
#define STOP_BUTTON     PB11 // 0: Disable STOP_BUTTON

Running LED   
#define RUNNING_LED     PB10 // 0: Disable RUNNING_LED


