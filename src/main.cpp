/*
Query a webcam_state_url to see if the webcam is on or off amd turn on or off the LED strip accordingly
Example webcam_state_url can be found at: https://github.com/danbowker/WebCamcam_state_url
Runs on ESP32 or ESP8266 and works with RGB LED strip.
*/

#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#endif

#include <mDNSResolver.h>

WiFiClient client;

WiFiUDP udp;
mDNSResolver::Resolver resolver(udp);

// SSID, PASSWORD and cam_state_url are #defined in connection.h
#include <connection.h>

#include <Adafruit_NeoPixel.h>

#define LED_COUNT 12
#define LED_PIN 15
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
bool on = false;

void setup()
{
  Serial.begin(115200);
  delay(100);

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(100);

  resolver.setLocalIP(WiFi.localIP());
  IPAddress ip = resolver.search("comaiz4");
  if (ip == INADDR_NONE)
  {
    Serial.println("Failed to resolve mDNS");
  }
  else
  {
    Serial.println("mDNS resolved to: " + ip.toString());
  }

  strip.begin();
  strip.setBrightness(100);
  delay(100);
}

void loop()
{
  resolver.loop();
  IPAddress ip = resolver.search("comaiz4.local");
  if (ip == INADDR_NONE)
  {
    Serial.println("Failed to resolve mDNS");
    return;
  }
  else
  {
    Serial.println("mDNS resolved to: " + ip.toString());
  }

  // Build cam_state_url using ip
  String cam_state_url_dns = "http://" + ip.toString() + ":5010/webcam";
  
  HTTPClient http;
  Serial.println("Checking webcam state at " + String(cam_state_url_dns));
  http.begin(client, cam_state_url);
  int httpCode = http.GET();
  Serial.println("HTTP code: " + String(httpCode));
  String result;
  if (httpCode > 0)
  {
    result = http.getString();
    Serial.println("Webcam state: " + result);
  }
  else
  {
    Serial.println("Error on HTTP request: " + http.errorToString(httpCode));
  }

  if (result == "true")
  {
    // Turn on
    if(!on)
    {
      Serial.println("Turning on");
      on = true;      
    }
    for (int i = 0; i < LED_COUNT; i++)
    {
      strip.setPixelColor(i, strip.Color(255, 0, 0));
    }
    strip.show();
  }
  else
  {
    // Turn off
    if(on)
    {
      Serial.println("Turning off");
      on = false;      
    }
    for (int i = 0; i < LED_COUNT; i++)
    {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
  }

  http.end();
  delay(2500);
}
