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

WiFiClient client;

// SSID, PASSWORD and cam_state_url are #defined in connection.h
#include <connection.h>

#include <Adafruit_NeoPixel.h>

#define HOSTNAME "webcam-light"
#define TARGET_HOSTNAME "comaiz4"

#define LED_COUNT 11
#define LED_PIN 15
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
bool on = false;

bool resolve_mdns_service(char* service_name, char* protocol, char* desired_host, IPAddress* ip_addr, uint16_t *port_number) {
  Serial.println("Sending mDNS query");
  int n = MDNS.queryService(service_name, protocol);
  Serial.printf("mDNS query got %d results\n", n);

  if(n == 0) {
    Serial.println("no services found");
  } else {
    for (int i = 0; i < n; ++i) {

      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(MDNS.hostname(i));
      Serial.print(" (");
      Serial.print(MDNS.IP(i));
      Serial.print(":");
      Serial.print(MDNS.port(i));
      Serial.println(")");

      if(strcmp(MDNS.hostname(i).c_str(), desired_host) == 0) {
        *ip_addr = MDNS.IP(i);
        *port_number = MDNS.port(i);
        return true;
      }
    }
  }

  return false;
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

  if(!MDNS.begin(HOSTNAME))
    Serial.println("Error setting up MDNS responder!");
  else
    Serial.println("mDNS responder started");

  strip.begin();
  strip.setBrightness(100);
  delay(100);
}

void loop()
{
  IPAddress server_ip;
  uint16_t port_number;

  if(resolve_mdns_service("http", "tcp", TARGET_HOSTNAME, &server_ip, &port_number)) {
    Serial.printf("got an answer for %s.local!\n", TARGET_HOSTNAME);
    Serial.println(server_ip);
    Serial.println(port_number);
  } else {
    Serial.printf("Sorry, %s.local not found\n", TARGET_HOSTNAME);
  }

  // Create camera state URL using resolved IP address
  String cam_state_url_resolved = "http://" + server_ip.toString() + ":" + String(port_number) + "/webcam";

  HTTPClient http;
  Serial.println("Checking webcam state at " + String(cam_state_url_resolved));
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

