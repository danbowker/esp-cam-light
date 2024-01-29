/*
Query a webserver to see if the webcam is on or off amd turn on or off the LED strip accordingly
Example webserver can be found at: https://github.com/danbowker/WebCamServer
Runs on ESP32 or ESP8266 and works with RGB LED strip.
*/

#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#endif

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
#endif

WiFiClient client;
#include <credentials.h>

// #define server "http://host.wokwi.internal:5010/webcam"
#define server "http://10.240.170.51:5010/webcam"

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

  strip.begin();
  strip.setBrightness(100);
  delay(100);
}

void loop() {
  HTTPClient http;
  http.begin(client, server);
  int httpCode = http.GET();
  String result;
  if (httpCode > 0) {
    result = http.getString();
  }
  else {
    Serial.println("Error on HTTP request: " + http.errorToString(httpCode));
  }
  
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

  http.end();
  delay(2500);
}

