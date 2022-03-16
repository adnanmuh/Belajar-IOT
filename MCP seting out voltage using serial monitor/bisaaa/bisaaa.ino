#include<Wire.h>                   //Include Wire library for using I2C functions 
#define MCP4725 0x60              //MCP4725 address as 0x61 Change yours accordingly
String input;
byte buffer[3];                   

void setup() 
{
  Serial.begin(9600);
  Wire.begin();                    //Begins the I2C communication
}

void loop() 

{
   while (Serial.available() == 0)   
    { /*Wait for user input*/ }  
  input = Serial.readString();       //Reading  Input string from Serial Monitor.  
  unsigned int adc= input.toInt();    //Convert String to Int
  buffer[0] = 0b01000000;            //Sets the buffer 0 with control byte (010-Sets in Write mode)
  float ipvolt = (5.0/4096.0)* adc;  //Finding voltage formula (A0)
  Serial.println(adc);
  Serial.println(ipvolt);
  buffer[1] = adc >> 4;              //Puts the most significant bit values
  buffer[2] = adc << 4;              //Puts the Least significant bit values
  
  Wire.beginTransmission(MCP4725);         //Joins I2C bus with MCP4725 with 0x61 address  
  Wire.write(buffer[0]);            //Sends the control byte to I2C 
  Wire.write(buffer[1]);            //Sends the MSB to I2C 
  Wire.write(buffer[2]);            //Sends the LSB to I2C
  Wire.endTransmission();           //Ends the transmission
  delay(250);
}
