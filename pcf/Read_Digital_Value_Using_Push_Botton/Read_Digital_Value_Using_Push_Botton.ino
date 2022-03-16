#include "Arduino.h"
#include "PCF8574.h"
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
unsigned long stopwatch;
// Set i2c HEX address
PCF8574 pcf8574(0x21);
unsigned long timeElapsed;

void setup(){
  Serial.begin(9600);
  pcf8574.pinMode(P0, INPUT);
  pcf8574.pinMode(P1, OUTPUT);
  pcf8574.begin();
}

void loop(){
  uint8_t val = pcf8574.digitalRead(P0);            // Read the value of pin P0
  if (val ==HIGH){
    Serial.println("1");
    startMillis = millis();  //initial start time}
    stopwatch= startMillis-currentMillis;
    Serial.println(String(stopwatch));
  }
  if (val ==LOW){Serial.println("0");
    currentMillis=startMillis;
    }
  delay(250);
}
