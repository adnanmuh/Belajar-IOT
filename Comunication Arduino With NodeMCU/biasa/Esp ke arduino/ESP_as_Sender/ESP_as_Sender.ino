#include<SoftwareSerial.h> //Included SoftwareSerial Library
SoftwareSerial s(3,1);
int x;
void setup() {
  //Serial S Begin at 9600 Baud
  s.begin(9600);
}

void loop() {
  //Write '123' to Serial
  x=random(1,4);
  s.write(x);
  delay(1000);
}
