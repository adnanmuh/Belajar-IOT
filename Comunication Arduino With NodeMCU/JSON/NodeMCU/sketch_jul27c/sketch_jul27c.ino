#include <SoftwareSerial.h>
SoftwareSerial s(D6,D5);
#include <ArduinoJson.h>
 int x,data1,data2;
void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  s.begin(9600);
  while (!Serial) continue;
  x=0;
}

void loop() {
 x++;
 StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);
  if (root == JsonObject::invalid())
    return;
  data1=root["data1"];
  data2=root["data2"];
  Serial.print("Data ke: ");
  Serial.println(x);
  Serial.print("Data 1 ");
  Serial.println(data1);
  Serial.print("Data 2 ");
  Serial.println(data2);  
}
