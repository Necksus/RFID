/**************************************************************************/
/*!
    @file     readMifareClassicIrq.pde
    @author   Adafruit Industries
	@license  BSD (see license.txt)

    This example will wait for any ISO14443A card or tag, and
    depending on the size of the UID will attempt to read from it.

    If the card has a 4-byte UID it is probably a Mifare
    Classic card, and the following steps are taken:

    Reads the 4 byte (32 bit) ID of a MiFare Classic card.
    Since the classic cards have only 32 bit identifiers you can stick
	them in a single variable and use that to compare card ID's as a
	number. This doesn't work for ultralight cards that have longer 7
	byte IDs!

    Note that you need the baud rate to be 115200 because we need to
	print out the data and read from the card at the same time!

This is an example sketch for the Adafruit PN532 NFC/RFID breakout boards
This library works with the Adafruit NFC breakout
  ----> https://www.adafruit.com/products/364

Check out the links above for our tutorials and wiring diagrams

This example is for communicating with the PN532 chip using I2C. Wiring
should be as follows:
  PN532 SDA -> SDA pin
  PN532 SCL -> SCL pin
  PN532 IRQ -> D2
  PN532 SDA -> 3.3v (with 2k resistor)
  PN532 SCL -> 3.3v (with 2k resistor)
  PN532 3.3v -> 3.3v
  PN532 GND -> GND

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!
*/
/**************************************************************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include "secret.h"
#include <ezBuzzer.h>


int melody[] =
{
    NOTE_F5, NOTE_B5
};
int noteDurations[] = 
{
    8, 12
};

int melody2[] =
{
    NOTE_F5, 0
};
int noteDurations2[] = 
{
    4, 4
};


// notes in the melody:
int melody1[] = {
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,
  NOTE_E5,
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
  NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
  NOTE_D5, NOTE_G5
};

// note durations: 4 = quarter note, 8 = eighth note, etc, also called tempo:
int noteDurations1[] = {
  8, 8, 4,
  8, 8, 4,
  8, 8, 8, 8,
  2,
  8, 8, 8, 8,
  8, 8, 8, 16, 16,
  8, 8, 8, 8,
  4, 4
};

int noteLength;
// <<<<<<<<<<<

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

const int BUZZER_PIN = 2;
const int ALARM_PIN = 3;
const int DELAY_BETWEEN_CARDS = 500;
long timeLastCardRead = 0;
boolean readerDisabled = false;
int irqCurr;
int irqPrev;

// This example uses the IRQ line, which is available when in I2C mode.
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
ezBuzzer buzzer(BUZZER_PIN);

void setup(void) {
  noteLength = sizeof(noteDurations) / sizeof(int);
  pinMode(ALARM_PIN, INPUT);
  Serial.begin(115200);
  Serial2.begin(115200);
  while (!Serial) delay(10); // for Leonardo/Micro/Zero

  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  startListeningToNFC();
}

void loop(void) {
  buzzer.loop();
 
  if (digitalRead(ALARM_PIN) != 0 && buzzer.getState() == BUZZER_IDLE) { // if stopped
    buzzer.playMelody(melody2, noteDurations2, noteLength); // playing
  }  

  if (readerDisabled) {
    if (millis() - timeLastCardRead > DELAY_BETWEEN_CARDS) {
      readerDisabled = false;
      startListeningToNFC();
    }
  } else {
    irqCurr = digitalRead(PN532_IRQ);

    // When the IRQ is pulled low - the reader has got something for us.
    if (irqCurr == LOW && irqPrev == HIGH)
    {
       handleCardDetected();
    }

    irqPrev = irqCurr;
  }
}

void startListeningToNFC() 
{
  // Reset our IRQ indicators
  irqPrev = irqCurr = HIGH;

  if (nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A)) 
  {
    // Serial.println("Card already present.");
    handleCardDetected();
  }
}

void handleCardDetected()
{
    uint8_t success = false;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

    // read the NFC tag's info
    success = nfc.readDetectedPassiveTargetID(uid, &uidLength);

    if (success) {
      // Display some basic information about the card
      //Serial.println("Found an ISO14443A card");
      //Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
      Serial.print("Found tag with Id: ");
      nfc.PrintHex(uid, uidLength);

      if (uidLength == 4)
      {
        // We probably have a Mifare Classic card ...
        uint32_t cardid = uid[0];
        cardid <<= 8;
        cardid |= uid[1];
        cardid <<= 8;
        cardid |= uid[2];
        cardid <<= 8;
        cardid |= uid[3];
        //Serial.print("Seems to be a Mifare Classic card #");
        //Serial.println(cardid);

        // Read block 4 with secret authentication key
        success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
        if (success)
        {
          uint8_t data[16];
          char str[4];

          // Try to read the contents of block 4
          success = nfc.mifareclassic_ReadDataBlock(4, data);
          if (success)
          {
            // Data seems to have been read ... spit it out
            //Serial.println("Reading Block 4:");

            Serial2.print("{\"PN532\":{\"UID\":\"");
            for(int i=0; i<16; i++)
            {
              sprintf(str,"%x",data[i]);
              Serial2.print(str);
            }
            Serial2.print("\", \"DATA\":\"\"}}\n");

            //nfc.PrintHexChar(data, 16);

            buzzer.playMelody(melody, noteDurations, noteLength);
            // Wait a bit before reading the card again
            //delay(1000);
          }
          else
          {
            Serial.println("Ooops ... unable to read the requested block.  Try another key?");
          }
        }
        else
        {
          Serial.println("Ooops ... authentication failed: Try another key?");
        }

      }
      Serial.println("");

      timeLastCardRead = millis();
    }

    // The reader will be enabled again after DELAY_BETWEEN_CARDS ms will pass.
    readerDisabled = true;
}
