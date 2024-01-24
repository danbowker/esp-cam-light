/*
 Get Field1 of a ThingSpeak Channel and Set State of
 WS2812 NeoPixel Strip using ESP32
 Requirements:
   * ESP32 Wi-Fi Device
   * Arduino 2.0.3+ IDE
   * Additional Boards URL: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   * Library: ThingSpeak by MathWorks
   * Library: Adafruit NeoPixel by Adafruit
 Created: Jan 25, 2023 by Hans Scharler (https://nothans.com)
*/

#include <WiFi.h>

WiFiClient client;
const char* SSID = "PinkEther";
const char* PASSWORD = "SwinyardHill";

#include <HTTPClient.h>
String server = "http://192.168.1.110:5010/webcam";
HTTPClient http;


#include <Adafruit_NeoPixel.h>
#define LED_COUNT 11
#define LED_PIN 15
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.mode(WIFI_STA);  
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  delay(100);

  http.begin(server.c_str());

  strip.begin();
  strip.setBrightness(100);
  delay(100);
}

void loop() {
  int httpCode = http.GET();
  String result = http.getString();

  Serial.println(result);

  if (result == "true") {
    //Turn on
    Serial.println("Turning on");
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(255,0,0));
    }
    strip.show();
  }
  else {
    //Turn off
    Serial.println("Turning off");
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(0,0,0));
    }
    strip.show();
  }
  delay(2500);
}
