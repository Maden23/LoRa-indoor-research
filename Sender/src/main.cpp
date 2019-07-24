#include <Arduino.h>
#include "heltec.h"

#define BAND    868E6  
#define MSG_COUNT 500
#define WAIT_BEFORE_SF_CHANGE 20000
#define WAIT_BETWEEN_MESSAGES 500

unsigned int counter = 1;
int sf = 7;

void display()
{
  Serial.println("Sending packet: " + String(counter) + ", SF: " + String(sf));
  Heltec.display->clear();  
  Heltec.display->drawString(0, 0, "Sent packet: ");
  Heltec.display->drawString(90, 0, String(counter));
  Heltec.display->drawString(0, 20, "SF: ");
  Heltec.display->drawString(90, 20, String(sf));
  Heltec.display->display();
}

void setup() {
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Enable*/, false /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Heltec.LoRa.setFrequency(BAND);
  Heltec.LoRa.setSpreadingFactor(sf);
  Serial.begin(9600);
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
}

void loop() {
  // Reached wanted message count on this spreading factor, moving to the next one
  if (counter > MSG_COUNT)
  {
    sf++;
    counter = 1;
    LoRa.setSpreadingFactor(sf);
    
    if (sf > 12)
    {
      // All data is sent using all spreading factors
      Heltec.display->clear();
      Heltec.display->drawString(0, 0, "Transmission complete");
      Heltec.display->display();
      while (true);
    }

    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Changing SF to " + String(sf));
    Heltec.display->display();
    delay(WAIT_BEFORE_SF_CHANGE); 
   }


  // Sending packet
  display();
  LoRa.beginPacket();
  LoRa.print("hello " + String(counter));
  LoRa.endPacket();
  delay(WAIT_BETWEEN_MESSAGES);
  counter++;

  
}