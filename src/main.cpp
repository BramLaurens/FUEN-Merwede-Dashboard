#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <FastLED.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <HardwareSerial.h>

#define houseleds_pin 23

#define HV1_PIN 2 
#define HV1_NUM_LEDS 53
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS  255

#define HV2_PIN 4
#define HV2_NUM_LEDS 12

#define HV3_PIN 16
#define HV3_NUM_LEDS 26

#define LV1_PIN 17
#define LV1_NUM_LEDS 8

#define LV2_PIN 5
#define LV2_NUM_LEDS 8

#define WINDLED_PIN 18
#define WINDLED_NUM_LEDS 8

#define BATTLED_PIN 19
#define BATTLED_NUM_LEDS 11

CRGB leds[HV1_NUM_LEDS];
CRGB leds2[HV2_NUM_LEDS];
CRGB leds3[HV3_NUM_LEDS];
CRGB lv_leds1[LV1_NUM_LEDS];
CRGB lv_leds2[LV2_NUM_LEDS];
CRGB wind_leds[WINDLED_NUM_LEDS];
CRGB batt_leds[BATTLED_NUM_LEDS];


// Network credentials
const char* ssid     = "FUEN-EV2A-1";
const char* password = "123456789";

// Variable to store the HTTP request
String header;

//Server object
AsyncWebServer server(80);

// Hardware serial object
HardwareSerial SerialUART(2);


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

// Animation variables
int chase1Index = 0;
unsigned long last1Update = 0;
const unsigned long interval1 = 10;

int chase2Index = 0;
unsigned long last2Update = 0;
const unsigned long interval2 = 80;

int chase3Index = 0;
unsigned long last3Update = 0;
const unsigned long interval3 = 80;

int chase4Index = 0;
unsigned long last4Update = 0;
const unsigned long interval4 = 80;

int chase5Index = 0;
unsigned long last5Update = 0;
const unsigned long interval5 = 80;

int chase6Index = 0;
unsigned long last6Update = 0;
const unsigned long interval6 = 80;

int chase7Index = 0;
unsigned long last7Update = 0;
const unsigned long interval7 = 80;


// Prototypes
void HTML_handler();
void stripBlue();
void stripOff();
void HV1_animation();
void HV2_animation();
void HV3_animation();
void lv1_animation();
void lv2_animation();
void wind_animation();
void batt_animation();





void setup() {
  Serial.begin(115200);
  SerialUART.begin(115200, SERIAL_8N1, 26, 27); // Initialize hardware serial on pins 26 (RX) and 27 (TX)
  Serial.println("Starting...");
  Serial.println("Sending test message to SerialUART...");
  SerialUART.print("RED");
  delay(500); // Wait for a second to ensure the message is sent

  // Initialize the output variables as outputs
  pinMode(output2, OUTPUT);
  pinMode(output27, OUTPUT);
  pinMode(houseleds_pin, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output2, LOW);
  digitalWrite(output27, LOW);

  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
    return;
  }

  delay(500);

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
  FastLED.addLeds<LED_TYPE, LV1_PIN, COLOR_ORDER>(lv_leds1, LV1_NUM_LEDS);
  FastLED.addLeds<LED_TYPE, LV2_PIN, COLOR_ORDER>(lv_leds2, LV2_NUM_LEDS);
  FastLED.addLeds<LED_TYPE, WINDLED_PIN, COLOR_ORDER>(wind_leds, WINDLED_NUM_LEDS);
  FastLED.addLeds<LED_TYPE, BATTLED_PIN, COLOR_ORDER>(batt_leds, BATTLED_NUM_LEDS);

  FastLED.setBrightness(BRIGHTNESS);

  HTML_handler();

}

void loop() {

  if(runState == "Running") {
    HV1_animation();  // Run the animation function
    HV2_animation();  // Run the animation function
    HV3_animation();  // Run the animation function
    lv1_animation();  // Run the animation function
    lv2_animation();  // Run the animation function
    wind_animation(); // Run the animation function
    batt_animation(); // Run the animation function
    digitalWrite(houseleds_pin, HIGH);  // Turn on the house LEDs
  }
}

void stripBlue() {
  // Set all LEDs to a static color (e.g., red)
  fill_solid(leds, HV1_NUM_LEDS, CRGB::Blue);  // Options: Red, Green, Blue, etc.
  fill_solid(leds2, HV2_NUM_LEDS, CRGB::Blue);  // Options: Red, Green, Blue, etc.
  fill_solid(leds3, HV3_NUM_LEDS, CRGB::Blue);  // Options: Red, Green, Blue, etc.
  fill_solid(lv_leds1, LV1_NUM_LEDS, CRGB::Blue);  // Options: Red, Green, Blue, etc.
  FastLED.show();  // Send data to the strip
}

void stripOff() {
  // Turn off all LEDs
  fill_solid(leds, HV1_NUM_LEDS, CRGB::Black);
  fill_solid(leds2, HV2_NUM_LEDS, CRGB::Black);
  fill_solid(leds3, HV3_NUM_LEDS, CRGB::Black);
  fill_solid(lv_leds1, LV1_NUM_LEDS, CRGB::Black);
  FastLED.show();
  digitalWrite(houseleds_pin, LOW);  // Turn off the house LEDs
}

void HV1_animation() {
  unsigned long currentMillis = millis();

  if (currentMillis - last1Update >= interval1) {
    last1Update = currentMillis;

    // Clear LEDs
    fadeToBlackBy(leds, HV1_NUM_LEDS, 40);

    // Set the chase pixel
    leds[chase1Index] = CRGB::Blue;

    // Show the new frame
    FastLED.show();

    // Advance the chase index
    chase1Index = (chase1Index + 1) % HV1_NUM_LEDS;
  }
}

void HV2_animation() {
  unsigned long currentMillis = millis();

  if (currentMillis - last2Update >= interval2) {
    last2Update = currentMillis;

    // Clear LEDs
    fadeToBlackBy(leds2, HV2_NUM_LEDS, 100);

    // Set the chase pixel
    leds2[chase2Index] = CRGB::Blue;

    // Show the new frame
    FastLED.show();

    // Advance the chase index
    chase2Index = (chase2Index + 1) % HV2_NUM_LEDS;
  }
}

void HV3_animation() {
  unsigned long currentMillis = millis();

  if (currentMillis - last3Update >= interval3) {
    last3Update = currentMillis;

    // Clear LEDs
    fadeToBlackBy(leds3, HV3_NUM_LEDS, 100);

    // Set the chase pixel
    leds3[chase3Index] = CRGB::Blue;

    // Show the new frame
    FastLED.show();

    // Advance the chase index
    chase3Index = (chase3Index + 1) % HV3_NUM_LEDS;
  }
}

void lv1_animation() {
  unsigned long currentMillis = millis();

  if (currentMillis - last4Update >= interval4) {
    last4Update = currentMillis;

    // Clear LEDs
    fadeToBlackBy(lv_leds1, LV1_NUM_LEDS, 100);

    // Set the chase pixel
    lv_leds1[chase4Index] = CRGB::Blue;

    // Show the new frame
    FastLED.show();

    // Advance the chase index
    chase4Index = (chase4Index + 1) % LV1_NUM_LEDS;
  }
}

void lv2_animation() {
  unsigned long currentMillis = millis();

  if (currentMillis - last5Update >= interval5) {
    last5Update = currentMillis;

    // Clear LEDs
    fadeToBlackBy(lv_leds2, LV2_NUM_LEDS, 100);

    // Set the chase pixel
    lv_leds2[chase5Index] = CRGB::Blue;

    // Show the new frame
    FastLED.show();

    // Advance the chase index
    chase5Index = (chase5Index + 1) % LV2_NUM_LEDS;
  }
}

void wind_animation() {
  unsigned long currentMillis = millis();

  if (currentMillis - last6Update >= interval6) {
    last6Update = currentMillis;

    // Clear LEDs
    fadeToBlackBy(wind_leds, WINDLED_NUM_LEDS, 100);

    // Set the chase pixel
    wind_leds[chase6Index] = CRGB::Blue;

    // Show the new frame
    FastLED.show();

    // Advance the chase index
    chase6Index = (chase6Index + 1) % WINDLED_NUM_LEDS;
  }
}

void batt_animation() {
  unsigned long currentMillis = millis();

  if (currentMillis - last7Update >= interval7) {
    last7Update = currentMillis;

    // Clear LEDs
    fadeToBlackBy(batt_leds, BATTLED_NUM_LEDS, 100);

    // Set the chase pixel
    batt_leds[chase7Index] = CRGB::Blue;

    // Show the new frame
    FastLED.show();

    // Advance the chase index
    chase7Index = (chase7Index + 1) % BATTLED_NUM_LEDS;
  }
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