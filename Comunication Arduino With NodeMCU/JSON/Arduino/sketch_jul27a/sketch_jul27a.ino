#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial s(5,6);
int x,y;
void setup() {
s.begin(9600);
}

void loop() {
 x=random(1,10);
 y=random(10,100);
 StaticJsonBuffer<1000> jsonBuffer;
 JsonObject& root = jsonBuffer.createObject();
  root["data1"] = x;
  root["data2"] = y;
if(s.available()>0)
{
 root.printTo(s);
}
}
