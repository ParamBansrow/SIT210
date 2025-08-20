#include <Wire.h>
#include <BH1750.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

BH1750 lightMeter;

// -------- Wi-Fi Credentials --------
char ssid[] = "Param";
char pass[] = "aus@2112";

// -------- IFTTT Setup --------
char server[] = "maker.ifttt.com";
String IFTTT_Key = "kApM9DIFqVHl_Fy2dtSlVnpGH3IKLsKCPLNI08SXTMj";  
String event_sunlight = "sunlight";  
String event_stop = "shadow";  

WiFiClient wifi;
HttpClient client = HttpClient(wifi, server, 80);

bool inSunlight = false;  // To track state

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Connect WiFi
  Serial.print("Connecting to WiFi...");
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }
  Serial.println("Connected!");

  // Initialize BH1750
  Wire.begin();
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 ready");
  } else {
    Serial.println("Error initializing BH1750");
  }
}

void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  // Define threshold (e.g., 100 lux = bright enough)
  int threshold = 100;

  if (lux > threshold && !inSunlight) {
    triggerIFTTT(event_sunlight);
    inSunlight = true;
  } else if (lux <= threshold && inSunlight) {
    triggerIFTTT(event_stop);
    inSunlight = false;
  }

  delay(5000); // check every 5 seconds
}

void triggerIFTTT(String eventName) {
  String url = "/trigger/" + eventName + "/with/key/" + IFTTT_Key;

  Serial.print("Requesting: ");
  Serial.println(url);

  client.beginRequest();
  client.get(url);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
}
