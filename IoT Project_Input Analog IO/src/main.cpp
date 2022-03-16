#include <Arduino.h>
#include "LittleFS.h"
#include "WiFiManager.h"
#include "webServer.h"
#include "updater.h"
#include "fetch.h"
#include "configManager.h"
#include "dashboard.h"
#include "timeSync.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "PCF8574.h"
#include <ESP8266WiFi.h>
PCF8574 pcf8574(0x21);
// Data wire is plugged into pin on the Arduino
#define ONE_WIRE_BUS0 5
#define ONE_WIRE_BUS1 0
#define ONE_WIRE_BUS2 2
#define ONE_WIRE_BUS3 14
#define ONE_WIRE_BUS4 12
#define ONE_WIRE_BUS5 13
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire0(ONE_WIRE_BUS0);
OneWire oneWire1(ONE_WIRE_BUS1);
OneWire oneWire2(ONE_WIRE_BUS2);
OneWire oneWire3(ONE_WIRE_BUS3);
OneWire oneWire4(ONE_WIRE_BUS4);
OneWire oneWire5(ONE_WIRE_BUS5);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors0(&oneWire0);
DallasTemperature sensors1(&oneWire1);
DallasTemperature sensors2(&oneWire2);
DallasTemperature sensors3(&oneWire3);
DallasTemperature sensors4(&oneWire4);
DallasTemperature sensors5(&oneWire5);
// arrays to hold device address
DeviceAddress insideThermometer0;
DeviceAddress insideThermometer1;
DeviceAddress insideThermometer2;
DeviceAddress insideThermometer3;
DeviceAddress insideThermometer4;
DeviceAddress insideThermometer5;
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
      pcf8574.digitalWrite(P1, 0);
      dash.data.alarm = 1;
    }
  }
  else if (String(alarm) == "off"){
    if (alarmStatus==1) {
      alarmStatus=0;
      pcf8574.digitalWrite(P1, 1);
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
    pcf8574.pinMode(P0, OUTPUT);//Wifi LED
    pcf8574.pinMode(P1, OUTPUT);//Alarm LED
    pcf8574.pinMode(P2, OUTPUT);//Buzzer
    pcf8574.digitalWrite(P0, HIGH);
    pcf8574.digitalWrite(P1, HIGH);
    pcf8574.digitalWrite(P2, HIGH);
    for (int i = 0; i <= 1; i++) {
      pcf8574.pinMode(i, OUTPUT);
      pcf8574.digitalWrite(i, 0);
    }
    pcf8574.begin();
    sensors0.begin();
    sensors1.begin();
    sensors2.begin();
    sensors3.begin();
    sensors4.begin();
    sensors5.begin();
    sensors0.setResolution(insideThermometer0, 9);
    sensors1.setResolution(insideThermometer1, 9);
    sensors2.setResolution(insideThermometer2, 9);
    sensors3.setResolution(insideThermometer3, 9);
    sensors4.setResolution(insideThermometer4, 9);
    sensors5.setResolution(insideThermometer5, 9);
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
      String macAddress = WiFiManager.macAddress();
      String jsonPubBoard1 = "[{\"key\": \"boardSerial\", \"value\": \"" + DSAddress0 + "\", \"type\": \"device_status\"}]";
      String jsonPubBoard2 = "[{\"key\": \"boardTemp\", \"value\": \"" + boardTemp + "\", \"type\": \"device_status\"}]";
      String jsonPubBoard3 = "[{\"key\": \"SSID\", \"value\": \"" + String(WiFiManager.SSID()) + "\", \"type\": \"device_status\"}]";
      String jsonPubBoard4 = "[{\"key\": \"RSSI\", \"value\": \"" + String(WiFiManager.RSSI()) + "\", \"type\": \"device_status\"}]";
      String jsonPubBoard5 = "[{\"key\": \"localIP\", \"value\": \"" + String(WiFiManager.localIP()) + "\", \"type\": \"device_status\"}]";
      String jsonPubBoard6 = "[{\"key\": \"macAddress\", \"value\": \"" + String(WiFiManager.macAddress()) + "\", \"type\": \"device_status\"}]";
      String jsonPubBoard7 = "[{\"key\": \"deviceLabel\", \"value\": \"" + String(configManager.data.projectName) + "\", \"type\": \"device_status\"}]";
      String boardSerial = DSAddress0;
      boardTemp.toCharArray(dash.data.boardTemp,32);
      boardSerial.toCharArray(dash.data.boardSerial,32);
      macAddress.toCharArray(dash.data.macAddress,32);
      //Serial.println(jsonPub);
      //postMqtt
      if (mqttClient.connected()){
        mqttClient.publish(mqttUsername, String(jsonPubBoard1).c_str());
        mqttClient.publish(mqttUsername, String(jsonPubBoard2).c_str());
        mqttClient.publish(mqttUsername, String(jsonPubBoard3).c_str());
        mqttClient.publish(mqttUsername, String(jsonPubBoard4).c_str());
        mqttClient.publish(mqttUsername, String(jsonPubBoard5).c_str());
        mqttClient.publish(mqttUsername, String(jsonPubBoard6).c_str());
        mqttClient.publish(mqttUsername, String(jsonPubBoard7).c_str());
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
        pcf8574.digitalWrite(P1, 0);
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
        pcf8574.digitalWrite(P1, 1);
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
    //Wifi Status
    switch (WiFi.status()){
          case WL_NO_SSID_AVAIL:
            Serial.println("Lost Connection Cek Your Connection");
            pcf8574.digitalWrite(P0, LOW);
            delay(50);
            pcf8574.digitalWrite(P2, HIGH);//
            delay(50);
            pcf8574.digitalWrite(P2, LOW);
            delay(50);
            pcf8574.digitalWrite(P0, HIGH);//
            delay(50);
            break;
          case WL_CONNECTED:
            Serial.println("Connection Succes");
            pcf8574.digitalWrite(P3, HIGH);
            pcf8574.digitalWrite(P0, LOW);
            break;
          case WL_CONNECT_FAILED:
            Serial.println("Connection failed");
            pcf8574.digitalWrite(P1, HIGH);
            delay(50);
            pcf8574.digitalWrite(P1, LOW);
            delay(50);
            break;
        }
    // temp sensor function
    String jsonPub;
    int length;
    jsonPub = "";
    for (int i = 0;  i < deviceCount;  i++) {
      if (sensors1.getAddress(insideThermometer1, i)) {
        sensors1.requestTemperatures();
        String DSAddress1 = printAddress(insideThermometer1);
        float tempC1 = sensors1.getTempC(insideThermometer1);
        jsonPub += "\"" + DSAddress1 + "\":" + String(DallasTemperature::toFahrenheit(tempC1)) + ", ";
      }
    }
    length = jsonPub.length();
    if (length > 0) {
      jsonPub.remove(length-2,2);
    }
    String oneWire1s = jsonPub;
    oneWire1s.toCharArray(dash.data.oneWire1,192);
    jsonPub = "{" + jsonPub + "}";
    Serial.print("oneWire1");
    Serial.println(jsonPub);
    //postMqtt
    if (jsonPub!="{}"){
      if (mqttClient.connected()){
        mqttClient.publish(mqttUsername, String(jsonPub).c_str());
        mqttStatus = "Connected";
        mqttStatus.toCharArray(dash.data.mqttStatus,32);
      }
      else {
        reconnect();
      }
    }
    jsonPub = "";
    for (int i = 0;  i < deviceCount;  i++) {
      if (sensors2.getAddress(insideThermometer2, i)) {
        sensors2.requestTemperatures();
        String DSAddress2 = printAddress(insideThermometer2);
        float tempC2 = sensors2.getTempC(insideThermometer2);
        jsonPub += "\"" + DSAddress2 + "\":" + String(DallasTemperature::toFahrenheit(tempC2)) + ", ";
      }
    }
    length = jsonPub.length();
    if (length > 0) {
      jsonPub.remove(length-2,2);
    }
    String oneWire2s = jsonPub;
    oneWire2s.toCharArray(dash.data.oneWire2,192);
    jsonPub = "{" + jsonPub + "}";
    Serial.print("oneWire2");
    Serial.println(jsonPub);
    //postMqtt
    if (jsonPub!="{}"){
      if (mqttClient.connected()){
        mqttClient.publish(mqttUsername, String(jsonPub).c_str());
        mqttStatus = "Connected";
        mqttStatus.toCharArray(dash.data.mqttStatus,32);
      }
      else {
        reconnect();
      }
    }
    jsonPub = "";
    for (int i = 0;  i < deviceCount;  i++) {
      if (sensors3.getAddress(insideThermometer3, i)) {
        sensors3.requestTemperatures();
        String DSAddress3 = printAddress(insideThermometer3);
        float tempC3 = sensors3.getTempC(insideThermometer3);
        jsonPub += "\"" + DSAddress3 + "\":" + String(DallasTemperature::toFahrenheit(tempC3)) + ", ";
      }
    }
    length = jsonPub.length();
    if (length > 0) {
      jsonPub.remove(length-2,2);
    }
    String oneWire3s = jsonPub;
    oneWire3s.toCharArray(dash.data.oneWire3,192);
    jsonPub = "{" + jsonPub + "}";
    Serial.print("oneWire3");
    Serial.println(jsonPub);
    //postMqtt
    if (jsonPub!="{}"){
      if (mqttClient.connected()){
        mqttClient.publish(mqttUsername, String(jsonPub).c_str());
        mqttStatus = "Connected";
        mqttStatus.toCharArray(dash.data.mqttStatus,32);
      }
      else {
        reconnect();
      }
    }
    jsonPub = "";
    for (int i = 0;  i < deviceCount;  i++) {
      if (sensors4.getAddress(insideThermometer4, i)) {
        sensors4.requestTemperatures();
        String DSAddress4 = printAddress(insideThermometer4);
        float tempC4 = sensors4.getTempC(insideThermometer4);
        jsonPub += "\"" + DSAddress4 + "\":" + String(DallasTemperature::toFahrenheit(tempC4)) + ", ";
      }
    }
    length = jsonPub.length();
    if (length > 0) {
      jsonPub.remove(length-2,2);
    }
    String oneWire4s = jsonPub;
    oneWire4s.toCharArray(dash.data.oneWire4,192);
    jsonPub = "{" + jsonPub + "}";
    Serial.print("oneWire4");
    Serial.println(jsonPub);
    //postMqtt
    if (jsonPub!="{}"){
      if (mqttClient.connected()){
        mqttClient.publish(mqttUsername, String(jsonPub).c_str());
        mqttStatus = "Connected";
        mqttStatus.toCharArray(dash.data.mqttStatus,32);
      }
      else {
        reconnect();
      }
    }
    jsonPub = "";
    for (int i = 0;  i < deviceCount;  i++) {
      if (sensors5.getAddress(insideThermometer5, i)) {
        sensors5.requestTemperatures();
        String DSAddress5 = printAddress(insideThermometer5);
        float tempC5 = sensors5.getTempC(insideThermometer5);
        jsonPub += "\"" + DSAddress5 + "\":" + String(DallasTemperature::toFahrenheit(tempC5)) + ", ";
      }
    }
    length = jsonPub.length();
    if (length > 0) {
      jsonPub.remove(length-2,2);
    }
    String oneWire5s = jsonPub;
    oneWire5s.toCharArray(dash.data.oneWire5,192);
    jsonPub = "{" + jsonPub + "}";
    Serial.print("oneWire5");
    Serial.println(jsonPub);
    //postMqtt
    if (jsonPub!="{}"){
      if (mqttClient.connected()){
        mqttClient.publish(mqttUsername, String(jsonPub).c_str());
        mqttStatus = "Connected";
        mqttStatus.toCharArray(dash.data.mqttStatus,32);
      }
      else {
        reconnect();
      }
    }
    delay(500);
}