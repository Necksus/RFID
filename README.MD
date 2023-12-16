

Define a tag reader on Home Automation :
    mosquitto_pub -h mosquito_host -t homeassistant/tag/ZigbeeTagReader1/config -m '{"topic": "zigbee2mqtt/ZigbeeTagReader1/action", "value_template": "{{ value_json.PN532.UID }}"}' -u username -P password


Arduino 16 (TX2) => CC2530 P02 (UART)
Arduino 20 (SDA) => PN532 (SDA)
Arduino 21 (SCL) => PN532 (SCL)
