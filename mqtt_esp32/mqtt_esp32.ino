/*
  Basic MQTT example

  This sketch demonstrates the basic capabilities of the library.
  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.

*/
#include <WiFi.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "K";
const char* password = "kecilkali";
const char* mqtt_server = "iot-telemetry.wowrack.com";
const char* mqtt_username = "a2440909-ad56-4210-891f-4ba42cf7731a";
const char* mqtt_password = "qvb1YFeg8q67LgpGfCMDy5NXnx2Hgp5oa8RceHYpaAFahogSdbHyiVtfu02pY5Ua36V3wVQcfszhnaYF";


WiFiClient espClient;
PubSubClient client(espClient);



void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
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


  const char* setmode = doc["setmode"];
  if (String(setmode) == "1") {
    Serial.println("Alarm On");
  }
  else if (String(setmode) == "0") {
    Serial.println("Alarm Off");
  }
}


void setup() {
  Serial.begin(115200);x
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);
  if (client.connect(mqtt_username, mqtt_username, mqtt_password)) {
    client.subscribe(mqtt_username);
  }
}


void loop() {
  client.loop();
  int angka = random(1, 10);
  String stringangka = String(angka);
  String jsonData = "{\"key\":\"key1\", \"value\": \"satu\"}";
  String JsonDataSensor = "{\"boardSerial\":\"" + stringangka + "\"}";
  if (client.connected()) {
    client.publish(mqtt_username, String(jsonData).c_str());
    client.publish(mqtt_username, String(JsonDataSensor).c_str());
  }
  delay(1000);
}
