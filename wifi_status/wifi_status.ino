#include <ESP8266WiFi.h>
#include "Arduino.h"

// Replace with your network credentials
const char* ssid = "iPhone";
const char* password = "rahasiabanget";

const unsigned char Buzzer_With_LED = 0;
const unsigned char LED = 2;


void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    digitalWrite(LED, HIGH) ;
    delay(50);
    digitalWrite(LED, LOW) ;
    delay(50);
    digitalWrite(LED, HIGH) ;
    delay(50);
    digitalWrite(LED, LOW) ;
    delay(50);
  }
  Serial.println(WiFi.localIP());
  //The ESP8266 tries to reconnect automatically when the connection is lost
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void setup() {
  Serial.begin(115200);
  pinMode (Buzzer_With_LED, OUTPUT) ;
  pinMode (LED, OUTPUT) ;
  digitalWrite(LED, LOW) ;
  digitalWrite(Buzzer_With_LED, LOW) ;
  initWiFi();
}

void loop() {
    switch (WiFi.status()){
      case WL_NO_SSID_AVAIL:
        Serial.println("Configured SSID cannot be reached");
        digitalWrite(LED, LOW) ;
        digitalWrite(Buzzer_With_LED, HIGH) ;
        delay (50);
        digitalWrite(Buzzer_With_LED, LOW) ; 
        delay (50);
        break;
      case WL_CONNECTED:
        Serial.println("Connection successfully established");
        digitalWrite(Buzzer_With_LED, LOW) ;
        digitalWrite(LED, HIGH) ; 
        break;
      case WL_CONNECT_FAILED:
        Serial.println("Connection failed");
        digitalWrite(Buzzer_With_LED, LOW) ;
        delay (50);
        digitalWrite(LED, LOW) ;
        delay (50);
        digitalWrite(Buzzer_With_LED, HIGH) ;
        delay (50);
        digitalWrite(LED, HIGH) ;
        delay (50);        
        break;
    }
}
