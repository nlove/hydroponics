#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h> // MQTT protocol library
#include <ArduinoJson.h> // Format messages into JSON

// set networking values
const char* ssid = "IdentityTheft";
const char* password = "lemongrab";
const char* mqttServer = "10.42.0.1";
const char* username = "rick"; //mqtt user
const char* userpass = "reality"; //mqtt password

WiFiClient espClient;
PubSubClient client(espClient);
char sub[100];

void setup() {
  Serial.begin(115200);

  //connect to wifi
  wifiConnect();

  //Connect to the MQTT server and subscribe to the topic
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
  brokerConnect();

  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
}


void wifiConnect() {
  delay(15);
  Serial.print("WiFi settup...\n");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected to ");
  Serial.print(WiFi.localIP()); // the ip address of the WAP
  Serial.print("\n");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("\n");
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    sub[i] = ((char)payload[i]);
  }

  
  
  DynamicJsonBuffer jsonBuffer;

  // payload is the char[] generated by the callback function
  JsonObject& root = jsonBuffer.parseObject(sub);

  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  const char* publisher = root["node"];
  int temperature = root["temperature"];
  int humidity = root["humidity"];
  double ph = root["ph"];
  int ppm = root["ppm"];

  if (ph > 9) {
    digitalWrite(D1, HIGH);
  }

  if (ph < 5) {
    digitalWrite(D2, HIGH);
  }

  if (ph > 5 && ph < 9) {
    digitalWrite(D1, LOW);
    digitalWrite(D2, LOW);
  }

  Serial.print("\n\nDHT11: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println("%");
  Serial.print("\n");

  Serial.print("pH: ");
  Serial.print((double)ph);
  Serial.print(", PPM: "); 
  Serial.print((int)ppm);
  
  Serial.println();
}

void brokerConnect() {
  //loop until connected to the server
  while (!client.connected()) {
    Serial.print("MQTT setup...\n");
    if (client.connect("Plant-Node-2", username, userpass)) {
      Serial.println("MQTT connected\n");
      client.publish("plantTopic", "Hello Walls");
      client.subscribe("plantTopic"); // Subscribe to the topic
    }
  }
}

void loop() {

  // Check for WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnect();
  }

  // Check for Broker connection
  if (!client.connected()) {
    brokerConnect();
  } 

  client.loop(); 
}