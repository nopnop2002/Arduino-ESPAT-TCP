# Environment
You need to change the following environment.

- #define MQTT_SERVER     "broker.hivemq.com"   
MQTT server you use   
- #define SUB_TOPIC       "/ESP-AT-MQTT/#"   
MQTT Subscribe Topic   
- #define MAX_TOPIC       64   
MQTT Subscribe Topic Maximun Length   
- #define MAX_PAYLOAD     64   
MQTT Subscribe Payload Maximum Length   
- #define STOP_BUTTON     0   
Stop Button GPIO   
- define RUNNING_LED     13   
Running LED GPIO   


# Publish using mosquitto_pub
```
#!/bin/bash
#set -x
fail=0
while :
do
        payload=`date "+%Y/%m/%d %H:%M:%S"`
        echo ${payload}
        mosquitto_pub -h broker.hivemq.com -p 1883 -t "/ESP-AT-MQTT/text" -m "${payload}"
        if [ $? -ne 0 ]; then
                fail=$((++fail))
                echo ${fail}
                sleep 3
        fi
        sleep 1
done
```