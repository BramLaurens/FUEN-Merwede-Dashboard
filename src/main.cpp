#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <FastLED.h>
#include <ESPAsyncWebServer.h>

#define HV1_PIN 2 
#define HV1_NUM_LEDS 53
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS  255

#define HV2_PIN 4
#define HV2_NUM_LEDS 12

#define HV3_PIN 16
#define HV3_NUM_LEDS 26

CRGB leds[HV1_NUM_LEDS];
CRGB leds2[HV2_NUM_LEDS];
CRGB leds3[HV3_NUM_LEDS];

// Network credentials
const char* ssid     = "FUEN-EV2A-1";
const char* password = "123456789";

// Variable to store the HTTP request
String header;

//Server object
AsyncWebServer server(80);


// Buttonstates
String output2State = "off";
String output27State = "off";
String runState = "Stopped";
int sliderValue = 0;

// Simulation states
bool PV_ON = false;
bool wind_ON = false;
bool EV_ON = false;
bool WP_ON = false;
bool BMS_ON = false;
int weatherState = 0; // 0: Sunny, 1: Cloudy+wind, 2: Rainy
int simulationState = 0; // 0: Custom, 1: Overload, 2: regular

// Assign output variables to GPIO pins
const int output2 = 2; 
const int output27 = 27;

// Prototypes
void HTML_handler();
void stripBlue();
void stripOff();

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output2, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output2, LOW);
  digitalWrite(output27, LOW);

  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
    return;
  }

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  server.begin();

  FastLED.addLeds<LED_TYPE, HV1_PIN, COLOR_ORDER>(leds, HV1_NUM_LEDS);
  FastLED.addLeds<LED_TYPE, HV2_PIN, COLOR_ORDER>(leds2, HV2_NUM_LEDS);
  FastLED.addLeds<LED_TYPE, HV3_PIN, COLOR_ORDER>(leds3, HV3_NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  HTML_handler();
}

void stripBlue() {
  // Set all LEDs to a static color (e.g., red)
  fill_solid(leds, HV1_NUM_LEDS, CRGB::Blue);  // Options: Red, Green, Blue, etc.
  fill_solid(leds2, HV2_NUM_LEDS, CRGB::Blue);  // Options: Red, Green, Blue, etc.
  fill_solid(leds3, HV3_NUM_LEDS, CRGB::Blue);  // Options: Red, Green, Blue, etc.
  FastLED.show();  // Send data to the strip
}

void stripOff() {
  // Turn off all LEDs
  fill_solid(leds, HV1_NUM_LEDS, CRGB::Black);
  fill_solid(leds2, HV2_NUM_LEDS, CRGB::Black);
  fill_solid(leds3, HV3_NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void HTML_handler() {
 // Serve index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  // Serve style.css
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/style.css", "text/css");
  });

  // JSON endpoint for states
  server.on("/gpio", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "{";
    json += "\"runState\":\"" + runState + "\",";
    json += "\"2\":\"" + output2State + "\",";
    json += "\"27\":\"" + output27State + "\"";
    json += "}";
    request->send(200, "application/json", json);
    Serial.println("GPIO JSON sent: " + json);
  });

  // Handle run_state
  server.on("/run_state/run", HTTP_GET, [](AsyncWebServerRequest *request) {
    runState = "Running";
    stripBlue();
    request->send(200, "text/plain", "OK");
  });

  server.on("/run_state/stop", HTTP_GET, [](AsyncWebServerRequest *request) {
    runState = "Stopped";
    stripOff();
    request->send(200, "text/plain", "OK");
  });

  // GPIO control
  server.on("/2/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    output2State = "on";
    digitalWrite(output2, HIGH);
    request->send(200, "text/plain", "OK");
  });

  server.on("/2/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    output2State = "off";
    digitalWrite(output2, LOW);
    request->send(200, "text/plain", "OK");
  });

  server.on("/27/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    output27State = "on";
    digitalWrite(output27, HIGH);
    request->send(200, "text/plain", "OK");
  });

  server.on("/27/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    output27State = "off";
    digitalWrite(output27, LOW);
    request->send(200, "text/plain", "OK");
  });

  // Slider value endpoint
  server.on("/set_value", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("value")) {
      sliderValue = request->getParam("value")->value().toInt();
      sliderValue = constrain(sliderValue, 0, 255);
      Serial.printf("Slider value: %d\n", sliderValue);
    }
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}