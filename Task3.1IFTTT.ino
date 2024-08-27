#include <Wire.h>
#include <BH1750.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

BH1750 lightMeter;

const char ssid[] = "S22";
const char pass[] = "11223344..";
const char iftttUrl[] = "maker.ifttt.com";
const char iftttEvent[] = "/trigger/luminous/with/key/gHWY9aom5EE87-hAYjkGEoV2nrB7hVisd7Y8-wXflVr";

WiFiClient wifiClient;
HttpClient client = HttpClient(wifiClient, iftttUrl, 80);

void setup() {
  Serial.begin(9600);
  
  // Initialize I2C communication for the light sensor
  Wire.begin();
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("Error initializing BH1750!"));
    while (1);
  }

  // Connect to WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
}

void loop() {
  float lux = lightMeter.readLightLevel(); // Get light level in lux
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  
  // Define a light threshold for sending notifications
  if (lux > 100.0) {  // Example threshold, adjust as needed
    sendIFTTTTrigger("Light Detected");
    delay(10000); // Wait for 10 seconds to avoid spamming
  } else {
    sendIFTTTTrigger("Not Detected");
    delay(10000);
  }
}

void sendIFTTTTrigger(const char* message) {
  client.beginRequest();
  client.post(iftttEvent);
  client.sendHeader("Content-Type", "application/json");
  
  // Prepare the JSON payload to send the message to IFTTT
  String payload = "{\"value1\":\"";
  payload += message;
  payload += "\"}";
  
  client.sendHeader("Content-Length", payload.length());
  client.beginBody();
  client.print(payload);
  client.endRequest();
  
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.println("Status code: " + String(statusCode));
  Serial.println("Response: " + response);
}