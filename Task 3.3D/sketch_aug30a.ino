#include <WiFiNINA.h>
#include <PubSubClient.h>

// ====== WiFi credentials ======
const char* ssid = "Param";
const char* password = "aus@2112";

// ====== MQTT Broker details ======
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* topic = "SIT210/wave";

// ====== Hardware Pins ======
const int trigPin = 9;
const int echoPin = 10;
const int ledPin = 7;

// ====== MQTT Client ======
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// ====== Function Prototypes ======
void setup_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);
long readDistance();

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long distance = readDistance();
  if (distance > 0 && distance < 20) {   // within 20cm = wave detected
    Serial.println("Wave detected! Publishing...");
    client.publish(topic, "Param");   // <-- put your name
    delay(1000);
  }
}

// ====== Functions ======

void setup_wifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    String clientId = "Nano33IoT-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message received: ");
  Serial.println(message);

  if (message.equals("wave")) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
    }
  } 
  else if (message.equals("pat")) {
    // Different blink pattern
    for (int i = 0; i < 2; i++) {
      digitalWrite(ledPin, HIGH);
      delay(500);
      digitalWrite(ledPin, LOW);
      delay(500);
    }
  }
}


long readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}
