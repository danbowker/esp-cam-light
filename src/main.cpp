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

WiFiClient client;

// SSID, PASSWORD and cam_state_url are #defined in connection.h
#include <connection.h>

#include <Adafruit_NeoPixel.h>

#define LED_COUNT 11
#define LED_PIN 15
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
bool on = false;
String cam_state_url_host;
String cam_state_url_resolved;

void resolveHostInUrlToIp()
{
  cam_state_url_resolved = String(cam_state_url);
  int portIndex = cam_state_url_resolved.indexOf(":", 7);
  if (portIndex != -1)
  {
    cam_state_url_host = cam_state_url_resolved.substring(7, portIndex);
  }
  else
  {
    cam_state_url_host = cam_state_url_resolved.substring(7);
  }

  IPAddress serverIp;
  while (serverIp.toString() == "0.0.0.0")
  {
    Serial.println("Resolving host: " + cam_state_url_host);
    delay(250);
    serverIp = MDNS.queryHost(cam_state_url_host);
  }

  Serial.println("Resolved to: " + serverIp.toString());

  // and then rebulid the URL with the IP address by replacing the host name with the IP address
  cam_state_url_resolved.replace(cam_state_url_host, serverIp.toString());
}

void switchLight(bool turnOn)
{
  if (turnOn)
  {
    if (!on)
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
    if (on)
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
}

void setup()
{
  Serial.begin(115200);
  delay(100);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  delay(100);

  while (mdns_init() != ESP_OK)
  {
    delay(1000);
    Serial.println("Starting MDNS...");
  }

  strip.begin();
  strip.setBrightness(100);
  delay(100);

  resolveHostInUrlToIp();
}

void loop()
{
  HTTPClient http;
  Serial.println("Checking webcam state at " + cam_state_url_resolved);
  http.begin(client, cam_state_url_resolved);
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
    switchLight(false);
    Serial.println("Try and resolve the host again");
    resolveHostInUrlToIp();
  }

  if (result == "true")
  {
    switchLight(true);
  }
  else
  {
    switchLight(false);
  }

  http.end();
  delay(2500);
}
