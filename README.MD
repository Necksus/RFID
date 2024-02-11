Define a tag reader on Home Automation :
mosquitto_pub -h mosquito_host -t homeassistant/tag/ZigbeeTagReader1/config -m '{"topic": "zigbee2mqtt/ZigbeeTagReader1/action", "value_template": "{{ value_json.PN532.UID }}"}' -u username -P password



# Schema

## Arduino Mega

Arduino 16 (TX2) => CC2530 P02 (UART)
Arduino 20 (SDA) => PN532 (SDA)
Arduino 21 (SCL) => PN532 (SCL)

Arduino 2 (Digital) => Buzzer
Arduino 3 (Digital) => LED
Arduino 4 (Digital) => CC2530 (P04)
Arduino 5 (Digital) => CC2530 (P05)
Arduino 6 (Digital) => CC2530 (P06)


## Arduino Every

Arduino 8 (SDA/A4) => PN532 (SDA)
Arduino 9 (SCL/A5) => PN532 (SCL)

Arduino 16 (TX) => CC2530 P02 (UART)
Arduino 20 (D2) => CC2530 (P04)
Arduino 21 (D3) => CC2530 (P06)
Arduino 22 (D4) => CC2530 (P05)

Arduino 23 (D5) => Buzzer
Arduino 24 (D6) => LED


Open Home Assistant "Parameters / Automation and scenes", add an automation configured to trigger when Home Assistant is started, then publish this MQTT message:

```
service: mqtt.publish
data:
  qos: 0
  retain: false
  topic: homeassistant/tag/ZigbeeTagReader1/config
  payload: >-
    {"topic": "zigbee2mqtt/ZigbeeTagReader1/action", "value_template": "{{
    '{{value_json.PN532.UID}}' }}"}
```
