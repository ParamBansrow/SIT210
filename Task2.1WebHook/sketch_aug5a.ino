#include <SPI.h>
#include <WiFiNINA.h>
#include "DHT.h"
#include "ThingSpeak.h"

#define DHTPIN 2          
#define DHTTYPE DHT22     
char ssid[] = "Param";     
char pass[] = "aus@2112"; 

WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);

unsigned long myChannelNumber = 3027163;
const char *myWriteAPIKey = "V4Q1Y75RH3UF34CZ";

void setup() {
  Serial.begin(9600);
  dht.begin();
  WiFi.begin(ssid, pass);
  ThingSpeak.begin(client);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void loop() {
  float temp = dht.readTemperature();

  if (isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.println(temp);

  ThingSpeak.setField(1, temp);
  int status = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (status == 200) {
    Serial.println("Data sent to ThingSpeak");
  } else {
    Serial.print("Error sending data: ");
    Serial.println(status);
  }

  delay(30000);
}
