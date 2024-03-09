Define a tag reader on Home Automation :
mosquitto_pub -h mosquito_host -t homeassistant/tag/ZigbeeTagReader1/config -m '{"topic": "zigbee2mqtt/ZigbeeTagReader1/action", "value_template": "{{ value_json.PN532.UID }}"}' -u username -P password

# Components


- 1 [Arduino Every](https://store.arduino.cc/products/arduino-nano-every)
- 1 [CC2530 + 2591](https://www.aliexpress.com/item/1005002387334208.html) Zigbee module
- 1 [DIP connector 10 (2x5) pins](https://www.aliexpress.com/item/1005004266492521.html)
- 1 [PN 532](https://www.aliexpress.com/item/32794353925.html) RFID module
- 1 [Piezo Buzzer](https://www.aliexpress.com/item/32948360045.html)
- 1 [Plastic box](https://www.aliexpress.com/item/1005002656654805.html) 92x58x23mm
- 1 [LED](https://www.aliexpress.com/item/4001192529762.html)



The Zigbee module can be flash with a [CC Debugger](https://www.aliexpress.com/item/32813122315.html), an [micro USB cable](https://www.aliexpress.com/item/1005003645398066.html) is needed to flash the Arduino module.


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
