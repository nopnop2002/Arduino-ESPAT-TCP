You can change these define.

MQTT server you use   
#define MQTT_SERVER     "192.168.10.40"   
//#define MQTT_SERVER     "broker.hivemq.com"   
//#define MQTT_SERVER     "iot.eclipse.org"   

MQTT Subscribe Topic   
#define SUB_TOPIC       "#"   

MQTT Subscribe Topic Maximun Length   
#define MAX_TOPIC       64

MQTT Subscribe Payload Maximum Length   
#define MAX_PAYLOAD     64

Stop Button   
#define STOP_BUTTON     PB11 // 0: Disable STOP_BUTTON

Running LED   
#define RUNNING_LED     PB10 // 0: Disable RUNNING_LED
