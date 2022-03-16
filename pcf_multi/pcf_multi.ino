#include "Arduino.h"
#include "PCF8574.h"

// Set the i2c HEX address
PCF8574 pcf8574(0x20);
String mati="HIGH";
String hidup="LOW";
void setup(){
  Serial.begin(115200);

  // Set the pinModes
  pcf8574.pinMode(P0, OUTPUT);
  pcf8574.pinMode(P1, OUTPUT);
  pcf8574.pinMode(P2, OUTPUT);
  pcf8574.pinMode(P3, OUTPUT);
  pcf8574.begin();
}

void loop(){
  pcf8574.digitalWrite(P0, 1);
  pcf8574.digitalWrite(P1, 1);
  pcf8574.digitalWrite(P2, 1);
//  delay(1000);
//  pcf8574.digitalWrite(P0, hidup);
//  pcf8574.digitalWrite(P1, hidup);
//  pcf8574.digitalWrite(P2, hidup);
//  delay(1000);
//  pcf8574.digitalWrite(P0, LOW);
//  pcf8574.digitalWrite(P1, HIGH);
//  pcf8574.digitalWrite(P2, LOW);
//  delay(1000);
//  pcf8574.digitalWrite(P0, LOW);
//  pcf8574.digitalWrite(P1, LOW);
//  pcf8574.digitalWrite(P2, HIGH);
  delay(1000);
}
