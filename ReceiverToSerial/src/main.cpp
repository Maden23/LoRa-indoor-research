#include <Arduino.h>
#include "heltec.h"
#include <WiFi.h>
#include <HTTPClient.h>

// Experiment settings
#define FLOOR 1
#define WAIT_BEFORE_SF_CHANGE 100000 //milliseconds

// LoRa settings
#define BAND    868E6  
int sf = 7;

// Spreadsheets settings
String url = "https://script.google.com/macros/s/AKfycbwpX2WmpByp9pHqo4lhtRgpG5lMd5f0ZewsTRSqKJq0c9crZ3Q/exec?";

// Variables
int packet_id = 0;
int rssi;
float snr;
unsigned long lastReceive;
unsigned long button_timer;
bool packetRecieved = false;


int parseLoRaPacket(int packetSize) {
  String packet;
  // for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  packet = LoRa.readString();
  packet_id = packet.substring(packet.lastIndexOf(' ')).toInt();
  rssi = LoRa.packetRssi();
  snr = LoRa.packetSnr();
}

void sendToSerial() {
  String request = url + "id=" + String(packet_id) + "&snr=" + String(snr) + 
                  "&rssi=" + String(rssi) + "&sf=" + String(sf) + "&floor=" + String(FLOOR);
  Serial.println(request);  
}

void displayInfo() {
  Heltec.display->clear();  
  Heltec.display->drawString(0, 0, "Received packet: ");
  Heltec.display->drawString(90, 0, String(packet_id));
  Heltec.display->drawString(0, 20, "SF: ");
  Heltec.display->drawString(90, 20, String(sf));
  Heltec.display->display();
}

void onReceive(int packetSize)
{
  parseLoRaPacket(packetSize);
  packetRecieved = true;
}

void setup() {
  Heltec.begin(true /*DisplayEnable Enable*/, true /*LoRa Enable*/, false /*Serial Disable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  // LoRa.setFrequency(BAND);
  Serial.begin(9600);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Waiting for packets...");
  Heltec.display->display();

  LoRa.setSpreadingFactor(sf);
  LoRa.onReceive(onReceive);
  LoRa.receive();
  lastReceive = millis();
}



void loop() {
  // int packetSize = LoRa.parsePacket();
  // if (packetSize) { 
  //   lastReceive = millis();
  //   parseLoRaPacket(packetSize); 
  //   displayInfo();
  //   sendToSerial();
  //   button_timer = millis();
  // }

  if (packetRecieved)
  {
    displayInfo();
    sendToSerial();
    packetRecieved = false;
    lastReceive = millis();
    button_timer = millis();
  }

  int btn = digitalRead(0);     

  if ((packet_id && millis() - lastReceive >= WAIT_BEFORE_SF_CHANGE) || (btn == 0 && millis() - button_timer >= 300))
  {
    LoRa.setSpreadingFactor(++sf);
    // LoRa.receive();  
    lastReceive = millis();
    button_timer = millis();
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Changed SF to " + String(sf));
    Heltec.display->display();
    Serial.println("Changed spreading factor to " + String(sf));
  }

  if (sf >= 13)
  {
    Serial.println("Transmission complete");
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Transmission complete");
    Heltec.display->display();
    while(true);
  }

}
  