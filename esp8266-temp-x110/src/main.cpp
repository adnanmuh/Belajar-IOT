#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "LittleFS.h"

#include "WiFiManager.h"
#include "webServer.h"
#include "updater.h"
#include "fetch.h"
#include "configManager.h"
#include "dashboard.h"
#include "timeSync.h"

#include <ESP8266WiFi.h>

#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "PCF8574.h"



// Data wire is plugged into pin on the Arduino
#define ONE_WIRE_BUS0 14
// #define ONE_WIRE_BUS1 0
// #define ONE_WIRE_BUS2 2
// #define ONE_WIRE_BUS3 14
// #define ONE_WIRE_BUS4 12
// #define ONE_WIRE_BUS5 13

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire0(ONE_WIRE_BUS0);
// OneWire oneWire1(ONE_WIRE_BUS1);
// OneWire oneWire2(ONE_WIRE_BUS2);
// OneWire oneWire3(ONE_WIRE_BUS3);
// OneWire oneWire4(ONE_WIRE_BUS4);
// OneWire oneWire5(ONE_WIRE_BUS5);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors0(&oneWire0);
// DallasTemperature sensors1(&oneWire1);
// DallasTemperature sensors2(&oneWire2);
// DallasTemperature sensors3(&oneWire3);
// DallasTemperature sensors4(&oneWire4);
// DallasTemperature sensors5(&oneWire5);

// arrays to hold device address
DeviceAddress insideThermometer0;
// DeviceAddress insideThermometer1;
// DeviceAddress insideThermometer2;
// DeviceAddress insideThermometer3;
// DeviceAddress insideThermometer4;
// DeviceAddress insideThermometer5;

int deviceCount = 15;
String jsonData = "";
WiFiClient espClient;
PubSubClient mqttClient(espClient);
const char* mqttServer;
const char* mqttUsername;
const char* mqttPassword;
String mqttStatus = "Disconnected";

// reset button
int buttonValue;
long buttonTimer = 0;
long longPressTime = 3000;
boolean buttonActive = false;
boolean longPressActive = false;

// env data delay
int period = 60000;
unsigned long time_now = 0;

// alarm + wifi
const unsigned char Buzzer_With_LED = 0;
const unsigned char LED = 2;
int alarmStatus = 0;

PCF8574 pcf8574satu(0x20);
PCF8574 pcf8574dua(0x22);
//PIN PCFke1
uint8_t Input1 = P0;
uint8_t Input2 = P1;
uint8_t Input3 = P2;
uint8_t Input4 = P3;
uint8_t Input5 = P4;
uint8_t Input6 = P5;
uint8_t Input7 = P6;
uint8_t Input8 = P7;
//PIN PCFke2
uint8_t Input9 = P0;
uint8_t Input10 = P1;
uint8_t Input11 = P2;
uint8_t Input12 = P3;
uint8_t Input13 = P4;
uint8_t Input14 = P5;
uint8_t Input15 = P6;
uint8_t Input16 = P7;

String stringval;
String stringval1;
String stringval2;
String stringval3;
String stringval4;
String stringval5;
String stringval6;
String stringval7;
String stringval8;
String stringval9;
String stringval10;
String stringval11;
String stringval12;
String stringval13;
String stringval14;
String stringval15;

struct task
{
    unsigned long rate;
    unsigned long previous;
};

task taskA = {.rate = 60000, .previous = 0};

//MQTT callabck
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  payload[length] = '\0';
  String message = (char*)payload;

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  const char* alarm = doc["alarm"];
  if (String(alarm) == "on"){
    if (alarmStatus==0) {
      alarmStatus=1;
      digitalWrite(Buzzer_With_LED, HIGH) ; 
      dash.data.alarm = 1;
    }
  }
  else if (String(alarm) == "off"){
    if (alarmStatus==1) {
      alarmStatus=0;
      digitalWrite(Buzzer_With_LED, LOW) ;
      dash.data.alarm = 0;
    }
  }
}

void saveCallback() {
    Serial.println("EEPROM saved"); 
}

void reconnect() {
  mqttServer = configManager.data.mqttServer;
  mqttUsername = configManager.data.mqttUsername;
  mqttPassword = configManager.data.mqttPassword;
  mqttStatus = "Disconnected";
  mqttStatus.toCharArray(dash.data.mqttStatus,32);
  //mqttClient.setServer(mqttServer, 1883);
  mqttClient.connect(mqttUsername, mqttUsername, mqttPassword);
  mqttClient.subscribe(mqttUsername);
}

void setup()
{
    Serial.begin(115200);
    pinMode (Buzzer_With_LED, OUTPUT) ;
    pinMode (LED, OUTPUT) ;
    digitalWrite(LED, LOW) ;
    digitalWrite(Buzzer_With_LED, LOW) ;
  //pcf 0x20
    pcf8574satu.pinMode(Input1, INPUT);
    pcf8574satu.pinMode(Input2, INPUT);
    pcf8574satu.pinMode(Input3, INPUT);
    pcf8574satu.pinMode(Input4, INPUT);
    pcf8574satu.pinMode(Input5, INPUT);
    pcf8574satu.pinMode(Input6, INPUT);
    pcf8574satu.pinMode(Input7, INPUT);
    pcf8574satu.pinMode(Input8, INPUT);
  //pcf 0x22
    pcf8574dua.pinMode(Input9, INPUT);
    pcf8574dua.pinMode(Input10, INPUT);
    pcf8574dua.pinMode(Input11, INPUT);
    pcf8574dua.pinMode(Input12, INPUT);
    pcf8574dua.pinMode(Input13, INPUT);
    pcf8574dua.pinMode(Input14, INPUT);
    pcf8574dua.pinMode(Input15, INPUT);
    pcf8574dua.pinMode(Input16, INPUT);
  
    pcf8574satu.begin();
    pcf8574dua.begin();
    
    sensors0.begin();
    // sensors1.begin();
    // sensors2.begin();
    // sensors3.begin();
    // sensors4.begin();
    // sensors5.begin();
    sensors0.setResolution(insideThermometer0, 9);
    // sensors1.setResolution(insideThermometer1, 9);
    // sensors2.setResolution(insideThermometer2, 9);
    // sensors3.setResolution(insideThermometer3, 9);
    // sensors4.setResolution(insideThermometer4, 9);
    // sensors5.setResolution(insideThermometer5, 9);

    LittleFS.begin();
    GUI.begin();
    configManager.begin();
    configManager.setConfigSaveCallback(saveCallback);
    WiFiManager.begin(configManager.data.projectName);
    //WiFiManager.forget();
    timeSync.begin();
    dash.begin(500);

    mqttServer = configManager.data.mqttServer;
    mqttUsername = configManager.data.mqttUsername;
    mqttPassword = configManager.data.mqttPassword;
    mqttClient.setServer(mqttServer, 1883);
    mqttClient.setCallback(mqttCallback);
    if (mqttClient.connect(mqttUsername,  mqttUsername, mqttPassword)) {
      mqttClient.subscribe(mqttUsername);
      mqttStatus = "Connected";
      mqttStatus.toCharArray(dash.data.mqttStatus,32);
    }
    else {
      mqttStatus = "Disconnected";
      mqttStatus.toCharArray(dash.data.mqttStatus,32);
    }

}

// function to print a device address
String printAddress(DeviceAddress deviceAddress)
{
  String dataString = "";
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) {
      //Serial.print("0");
      dataString += "0";
    }
    //Serial.print(deviceAddress[i], HEX);
    dataString += String(deviceAddress[i], HEX);
  }
  //Serial.print("-");
  //Serial.print(dataString);
  return dataString;
}

void loop()
{
    //software interrupts
    WiFiManager.loop();
    updater.loop();
    configManager.loop();
    dash.loop();
    mqttClient.loop();
    
    //reset function
    buttonValue = analogRead(A0);
    if (buttonValue > 900) {
      if (buttonActive == false) {
        buttonActive = true;
        buttonTimer = millis();
      }
      if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
        longPressActive = true;
        Serial.print("reset pressed");
        configManager.reset();
        WiFiManager.forget();
      }
    }
    else {
      if (buttonActive == true) {
        if (longPressActive == true) {
          longPressActive = false;
        }
        buttonActive = false;
      }
    }

    if(millis() >= time_now + period){
      time_now += period;
      mqttServer = configManager.data.mqttServer;

      //board temperature + network info
      sensors0.getAddress(insideThermometer0, 0);
      sensors0.requestTemperatures();
      String DSAddress0 = printAddress(insideThermometer0);
      float tempC0 = sensors0.getTempC(insideThermometer0);
      String boardTemp = String(DallasTemperature::toFahrenheit(tempC0));
      String jsonPubBoard = "{\"boardSerial\":\"" + DSAddress0 + "\", \"boardTemp\":" + boardTemp + ", \"SSID\":\"" + String(WiFiManager.SSID()) + "\", \"RSSI\":\"" + String(WiFiManager.RSSI()) + "\", \"localIP\":\"" + String(WiFiManager.localIP()) + "\", \"deviceLabel\": \"" + String(configManager.data.projectName) + "\"}";
      String boardSerial = DSAddress0;
      boardTemp.toCharArray(dash.data.boardTemp,32);
      boardSerial.toCharArray(dash.data.boardSerial,32);
      //Serial.println(jsonPub);
      //postMqtt
      if (mqttClient.connected()){
        mqttClient.publish(mqttUsername, String(jsonPubBoard).c_str());
        mqttStatus = "Connected";
        mqttStatus.toCharArray(dash.data.mqttStatus,32);
      }
      else {
        reconnect();
      }
      
    }

    // alarm function
    if (dash.data.alarm==1) {
      if (alarmStatus==0) {
        alarmStatus=1;
        digitalWrite(Buzzer_With_LED, LOW) ; 
        Serial.println("alarm On");
        if (mqttClient.connected()){
          String jsonPub = "{\"alarm\":\"on\"}";
          mqttClient.publish(mqttUsername, String(jsonPub).c_str());
        }
        else {
          reconnect();
        }
      }
    }
    else if (dash.data.alarm==0)
    {
      if (alarmStatus==1) {
        alarmStatus=0;
        digitalWrite(Buzzer_With_LED, HIGH) ;
        Serial.println("alarm Off");
        if (mqttClient.connected()){
          String jsonPub = "{\"alarm\":\"off\"}";
          mqttClient.publish(mqttUsername, String(jsonPub).c_str());
        }
        else {
          reconnect();
        }
      }
    }

    switch (WiFi.status()){
      case WL_NO_SSID_AVAIL:
        Serial.println("Lost Connection Cek Your Connection");
        digitalWrite(LED, LOW) ;
        digitalWrite(Buzzer_With_LED, HIGH) ;
        delay (50);
        digitalWrite(Buzzer_With_LED, LOW) ; 
        delay (50);
        break;
      case WL_CONNECTED:
        Serial.println("Connection Succes");
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

    
    uint8_t val = pcf8574satu.digitalRead(Input1);
    if (val==HIGH){
      stringval = "ON";
      stringval.toCharArray(dash.data.digitalIn,32);
    }
    else if (val==LOW)
    {
      stringval = "OFF";
      stringval.toCharArray(dash.data.digitalIn,32);
    }
    else{}      
    uint8_t val1 = pcf8574satu.digitalRead(Input2);
    if (val1==HIGH){
      stringval1 = "ON";
      stringval1.toCharArray(dash.data.digitalIn1,32);
    }
    else if (val1==LOW)
    {
      stringval1 = "OFF";
      stringval1.toCharArray(dash.data.digitalIn1,32);
    }
    else{} 
    uint8_t val2 = pcf8574satu.digitalRead(Input3);
    if (val2==HIGH){
      stringval2 = "ON";
      stringval2.toCharArray(dash.data.digitalIn2,32);
    }
    else if (val2==LOW)
    {
      stringval2 = "OFF";
      stringval2.toCharArray(dash.data.digitalIn2,32);
    }
    else{} 
    uint8_t val3 = pcf8574satu.digitalRead(Input4);
    uint8_t val4 = pcf8574satu.digitalRead(Input5);
    uint8_t val5 = pcf8574satu.digitalRead(Input6);
    uint8_t val6 = pcf8574satu.digitalRead(Input7);
    uint8_t val7 = pcf8574satu.digitalRead(Input8);

    uint8_t val8 = pcf8574dua.digitalRead(Input9);        
    uint8_t val9 = pcf8574dua.digitalRead(Input10);
    uint8_t val10 = pcf8574dua.digitalRead(Input11);
    uint8_t val11 = pcf8574dua.digitalRead(Input12);
    uint8_t val12 = pcf8574dua.digitalRead(Input13);
    uint8_t val13 = pcf8574dua.digitalRead(Input14);
    uint8_t val14 = pcf8574dua.digitalRead(Input15);
    uint8_t val15 = pcf8574dua.digitalRead(Input16);

    delay(500);
}