#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "LittleFS.h"
#include "WiFiManager.h"
#include "webServer.h"
#include "updater.h"
#include "fetch.h"
#include "configManager.h"
#include "timeSync.h"
char input;
char adc; 
void smartConfig()
{
  WiFi.mode(WIFI_STA);
  Serial.println("\r\nWait for Smartconfig waiting for connection" );
  delay(2000);
  //Wait for the network
  WiFi.beginSmartConfig();
  while (1)
  {
    Serial.print(".");
    delay(500);
    if (WiFi.smartConfigDone())
    {
      Serial.println("SmartConfig Success");
      Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      WiFi.setAutoConnect(true);  //set automatic connection
      break;
    }
  }
}
void setup() 
{
    Serial.begin(115200);
    //smartConfig();
    LittleFS.begin();
    GUI.begin();
    configManager.begin();
    smartConfig();
    WiFiManager.begin(configManager.data.projectName);
    timeSync.begin();
    // configManager.reset();
    // WiFiManager.forget();
    // smartConfig();
    Serial.println("Hello world");
}
void loop() 
{
    //software interrupts
    WiFiManager.loop();
    updater.loop();
    configManager.loop();
    if(Serial.available()){
        input = Serial.read();
        adc= input; 
        Serial.print("You typed: " );
        Serial.println(input);
        if (adc=='1')
        {
          Serial.println("Reset");
          configManager.reset();
          WiFiManager.forget();
        }
        else{}
    }
    //your code here
}