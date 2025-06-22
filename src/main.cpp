#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <FastLED.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <HardwareSerial.h>
#include <usage_profiles.h>

#define houseleds_pin 23
#define smoke_pin 22

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

/* General idea:
- Kleuren voor spanning
- Snelheid ledjes voor transportcapaciteit / afnamae
- Vermogen update elk uur*/


// Network credentials
const char* ssid     = "FUEN-EV2A-1";
const char* password = "123456789";

// Variable to store the HTTP request
String header;

//Server object
AsyncWebServer server(80);

// Hardware serial object
// HardwareSerial SerialUART(2);


// Buttonstates
String output2State = "off";
String output27State = "off";
int sliderValue = 0;

// Simulation state variables
String runState = "Stopped";
String pvState = "off"; // PV state
String windState = "off"; // Wind state
String evState = "off"; // EV state
String hpState = "off"; // hp state
String battState = "off"; // batt state

// global variables for simulation
long power = 0; // Power consumption in W

int weatherState = 0; // 0: Sunny, 1: Cloudy+wind, 2: Rainy
int simulationState = 0; // 0: Custom, 1: Overload, 2: regular

// Assign output variables to GPIO pins
const int output2 = 2; 
const int output27 = 27;

// Animation variables
int chase1Index = 0;
unsigned long last1Update = 0;
unsigned long interval1 = 80;

int chase2Index = 0;
unsigned long last2Update = 0;
unsigned long interval2 = 80;

int chase3Index = 0;
unsigned long last3Update = 0;
unsigned long interval3 = 80;

int chase4Index = 0;
unsigned long last4Update = 0;
unsigned long interval4 = 80;

int chase5Index = 0;
unsigned long last5Update = 0;
unsigned long interval5 = 80;

int chase6Index = 0;
unsigned long last6Update = 0;
unsigned long interval6 = 80;

int chase7Index = 0;
unsigned long last7Update = 0;
unsigned long interval7 = 80;

bool HV1_animation_enabled = true;
bool HV2_animation_enabled = true;
bool HV3_animation_enabled = true;
bool lv1_animation_enabled = true;
bool lv2_animation_enabled = true;
bool wind_animation_enabled = true;
bool batt_animation_enabled = true;

// Simulation timing variables
unsigned long last_hourupdate = 0;
unsigned long simInterval = 1000; //Simulation speed in ms per step
int currentHour = 0;

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
void simRunning();
void simSteps();
void displayUART(char code, long value);
void animationspeedMap(long power);

HardwareSerial SerialUART(2); // Create a HardwareSerial object for SerialUART

void setup() {
  Serial.begin(115200);
  SerialUART.begin(115200, SERIAL_8N1, 26, 27); // Initialize hardware serial on pins 26 (RX) and 27 (TX)
  Serial.println("Starting...");

  // Initialize the output variables as outputs
  pinMode(houseleds_pin, OUTPUT);
  pinMode(smoke_pin, OUTPUT);
  // Set outputs to LOW

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
    // Run simulation if Running state is set
    simRunning();
  }
  else if(runState == "Stopped") {
    // Stop all animations and turn off LEDs
    stripOff();
  }

}
void displayUART(char code, long value) {
  // Send data to the display MCU via SerialUART
  SerialUART.print(code);
  SerialUART.println(value);
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
  fill_solid(lv_leds2, LV2_NUM_LEDS, CRGB::Black);
  fill_solid(wind_leds, WINDLED_NUM_LEDS, CRGB::Black);
  fill_solid(batt_leds, BATTLED_NUM_LEDS, CRGB::Black);
  FastLED.show();
  digitalWrite(houseleds_pin, LOW);  // Turn off the house LEDs
  digitalWrite(smoke_pin, LOW);  // Turn on the smoke pin

}

void HV1_animation() {
  unsigned long currentMillis = millis();

  if (currentMillis - last1Update >= interval1) {
    last1Update = currentMillis;

    // Clear LEDs
    fadeToBlackBy(leds, HV1_NUM_LEDS, 40);

    // Set the chase pixel
    leds[chase1Index] = CRGB::White;

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

  digitalWrite(houseleds_pin, HIGH);  // Turn on the house LEDs
  if (currentMillis - last4Update >= interval4) {
    last4Update = currentMillis;

    // Clear LEDs
    fadeToBlackBy(lv_leds1, LV1_NUM_LEDS, 100);

    // Set the chase pixel
    lv_leds1[chase4Index] = CRGB::Red;

    // Show the new frame
    FastLED.show();

    // Advance the chase index
    if(power >= 0) {
      chase4Index = (chase4Index + 1) % LV1_NUM_LEDS;
    }
    else {
      if(chase4Index == 0) {
        chase4Index = LV1_NUM_LEDS - 1;
      } else {
        chase4Index--;
      }
    }
  }
}

void lv2_animation() {
  unsigned long currentMillis = millis();

  digitalWrite(houseleds_pin, HIGH);  // Turn on the house LEDs

  if (currentMillis - last5Update >= interval5) {
    last5Update = currentMillis;

    // Clear LEDs
    fadeToBlackBy(lv_leds2, LV2_NUM_LEDS, 100);

    // Set the chase pixel
    lv_leds2[chase5Index] = CRGB::Red;

    // Show the new frame
    FastLED.show();

    // Advance the chase index
    if(power >= 0) {
      chase5Index = (chase5Index + 1) % LV2_NUM_LEDS;
    }
    else {
      if(chase5Index == 0) {
        chase5Index = LV2_NUM_LEDS - 1;
      } else {
        chase5Index--;
      }
    }
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
    if (chase6Index == 0) {
      chase6Index = WINDLED_NUM_LEDS - 1;
    } else {
      chase6Index--;
    }
  }
}

void batt_animation() {
  unsigned long currentMillis = millis();

  if (currentMillis - last7Update >= interval7) {
    last7Update = currentMillis;

    // Clear LEDs
    fadeToBlackBy(batt_leds, BATTLED_NUM_LEDS, 100);

    // Set the chase pixel
    

    // Show the new frame
    FastLED.show();

    // Advance the chase index
    if (currentHour >= 5 && currentHour <= 9 || currentHour >= 17 && currentHour <= 20) {
      // If time is between 5 and 9 OR 17 and 20 show discharge animation
      batt_leds[chase7Index] = CRGB::White;
      if(chase7Index == 0) {
        chase7Index = BATTLED_NUM_LEDS - 1;
      } else {
        chase7Index--;
      }
    }
    else {
      batt_leds[chase7Index] = CRGB::Red;
      chase7Index = (chase7Index + 1) % BATTLED_NUM_LEDS;
    }
  }
}

void animationspeedMap(long power) {
  // Map the power value to a speed for the animations

  if(power >= 0){  
    interval1 = map(power, 0, 9000000, 50, 1);
    interval2 = map(power, 0, 8000000, 80, 1);
    interval3 = map(power, 0, 8000000, 80, 1);
    interval4 = map(power, 0, 9000000, 80, 8);
    interval5 = map(power, 0, 9000000, 80, 8);
    interval6 = map(power, 0, 9000000, 80, 8);
    interval7 = map(power, 0, 9000000, 80, 8);
  }
  else{
    interval1 = map(power, 0, -10000000, 50, 1);
    interval2 = map(power, 0, -10000000, 80, 1);
    interval3 = map(power, 0, -10000000, 80, 1);
    interval4 = map(power, 0, -10000000, 80, 8);
    interval5 = map(power, 0, -10000000, 80, 8);
    interval6 = map(power, 0, -10000000, 80, 8);
    interval7 = map(power, 0, -10000000, 80, 8); 
  }

}

void simRunning() {
  
  // Simulation timing framework
  unsigned long currentTime = millis();

  simInterval = map(sliderValue, 255, 0, 500, 2000); // Map slider value to simulation speed

  if(currentTime - last_hourupdate > simInterval){
     currentHour++;
     simSteps();
     last_hourupdate = currentTime;
  }

  if(currentHour >= 24) {
    currentHour = 0; // Reset to 0 after 24 hours
  }

  // Turn on windmill
  if(windState == "On") {
    wind_animation_enabled = true; // Enable wind animation if windState is On
  }
  else {
    wind_animation_enabled = false; // Disable wind animation if windState is Off
  }

  // Animations
  if(HV1_animation_enabled) {
    HV1_animation();
  }
  else{
    fill_solid(leds, HV1_NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
  if(HV2_animation_enabled && power < 8500000) {
    HV2_animation();
  }
  else{
    fill_solid(leds2, HV2_NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
  if(HV3_animation_enabled && power < 8500000) {
    HV3_animation();
  }
  else{
    fill_solid(leds3, HV3_NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
  if(lv1_animation_enabled && power < 8500000) {
    lv1_animation();
  }
  else{
    fill_solid(lv_leds1, LV1_NUM_LEDS, CRGB::Black);
    digitalWrite(houseleds_pin, LOW);  // Turn on the house LEDs
    FastLED.show();
  }
  if(lv2_animation_enabled && power < 8500000) {
    lv2_animation();
  }
  else{
    fill_solid(lv_leds2, LV2_NUM_LEDS, CRGB::Black);
    digitalWrite(houseleds_pin, LOW);  // Turn on the house LEDs
    FastLED.show();
  }
  if(wind_animation_enabled) {
    wind_animation();
  }
  else {
    fill_solid(wind_leds, WINDLED_NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
  if(batt_animation_enabled) {
    batt_animation();
  }
  else {
    fill_solid(batt_leds, BATTLED_NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
}

void simSteps(){
  // Logic that runs every simulation step while simulation is running

  // Base scenario
  if(pvState == "off" && evState == "On" && hpState == "On" && battState == "off") {

    power = usage_profile_base[currentHour]; // Get the power consumption for the current hour

    HV1_animation_enabled = true;
    HV2_animation_enabled = true;
    HV3_animation_enabled = true;
    lv1_animation_enabled = true;
    lv2_animation_enabled = true;
    wind_animation_enabled = false;
    batt_animation_enabled = false;

    Serial.print("Current Hour: " + String(currentHour));
    Serial.print("  ");
    Serial.println("Current load: " + String(power) + "W");
    displayUART('P', power);
    displayUART('H', currentHour);

    animationspeedMap(power);

  }

  // Usage profile with PV
  if(pvState == "On" && evState == "On" && hpState == "On" && battState == "off") {

    power = usage_profile_with_PV[currentHour]; // Get the power consumption for the current hour

    HV1_animation_enabled = true;
    HV2_animation_enabled = true;
    HV3_animation_enabled = true;
    lv1_animation_enabled = true;
    lv2_animation_enabled = true;
    wind_animation_enabled = false;
    batt_animation_enabled = false;

    Serial.print("Current Hour: " + String(currentHour));
    Serial.print("  ");
    Serial.println("Current load: " + String(power) + "W");
    displayUART('P', power);
    displayUART('H', currentHour);

    animationspeedMap(power);
  }

  // Usage profile no HP base
  if(pvState == "off" && evState == "On" && hpState == "off" && battState == "off") {

    power = usage_profile_nohp_base[currentHour]; // Get the power consumption for the current hour

    HV1_animation_enabled = true;
    HV2_animation_enabled = true;
    HV3_animation_enabled = true;
    lv1_animation_enabled = true;
    lv2_animation_enabled = true;
    batt_animation_enabled = false;

    Serial.print("Current Hour: " + String(currentHour));
    Serial.print("  ");
    Serial.println("Current load: " + String(power) + "W");
    displayUART('P', power);
    displayUART('H', currentHour);

    animationspeedMap(power);
  }

  // Usage profile with PV and no HP
  if(pvState == "On" && evState == "On" && hpState == "off" && battState == "off") {

    power = usage_profile_nohp_with_PV[currentHour]; // Get the power consumption for the current hour

    HV1_animation_enabled = true;
    HV2_animation_enabled = true;
    HV3_animation_enabled = true;
    lv1_animation_enabled = true;
    lv2_animation_enabled = true;
    batt_animation_enabled = false;

    Serial.print("Current Hour: " + String(currentHour));
    Serial.print("  ");
    Serial.println("Current load: " + String(power) + "W");
    displayUART('P', power);
    displayUART('H', currentHour);

    animationspeedMap(power);
  }

  // Usage profile no HP no EV base
  if(pvState == "off" && evState == "off" && hpState == "off" && battState == "off") {

    power = usage_profile_nohp_noev_base[currentHour]; // Get the power consumption for the current hour

    HV1_animation_enabled = true;
    HV2_animation_enabled = true;
    HV3_animation_enabled = true;
    lv1_animation_enabled = true;
    lv2_animation_enabled = true;
    batt_animation_enabled = false;

    Serial.print("Current Hour: " + String(currentHour));
    Serial.print("  ");
    Serial.println("Current load: " + String(power) + "W");
    displayUART('P', power);
    displayUART('H', currentHour);

    animationspeedMap(power);
  }

  // Usage profile with PV and no HP and no EV
  if(pvState == "On" && evState == "off" && hpState == "off" && battState == "off") {

    power = usage_profile_nohp_noev_with_PV[currentHour]; // Get the power consumption for the current hour

    HV1_animation_enabled = true;
    HV2_animation_enabled = true;
    HV3_animation_enabled = true;
    lv1_animation_enabled = true;
    lv2_animation_enabled = true;
    batt_animation_enabled = false;

    Serial.print("Current Hour: " + String(currentHour));
    Serial.print("  ");
    Serial.println("Current load: " + String(power) + "W");
    displayUART('P', power);
    displayUART('H', currentHour);

    animationspeedMap(power);
  }

  // Usage profile with EV, HP, Battery and no PV
  if(pvState == "off" && evState == "On" && hpState == "On" && battState == "On") {

    power = usage_profile_base_BATT[currentHour]; // Get the power consumption for the current hour

    HV1_animation_enabled = true;
    HV2_animation_enabled = true;
    HV3_animation_enabled = true;
    lv1_animation_enabled = true;
    lv2_animation_enabled = true;
    batt_animation_enabled = true;

    Serial.print("Current Hour: " + String(currentHour));
    Serial.print("  ");
    Serial.println("Current load: " + String(power) + "W");
    displayUART('P', power);
    displayUART('H', currentHour);

    animationspeedMap(power);
  }

  // Usage profile with EV, HP, Battery and PV
  if(pvState == "On" && evState == "On" && hpState == "On" && battState == "On") {

    power = usage_profile_with_PV_BATT[currentHour]; // Get the power consumption for the current hour

    HV1_animation_enabled = true;
    HV2_animation_enabled = true;
    HV3_animation_enabled = true;
    lv1_animation_enabled = true;
    lv2_animation_enabled = true;
    batt_animation_enabled = true;

    Serial.print("Current Hour: " + String(currentHour));
    Serial.print("  ");
    Serial.println("Current load: " + String(power) + "W");
    displayUART('P', power);
    displayUART('H', currentHour);

    animationspeedMap(power);
  }

  // Usage profile with HP, no EV, no PV
  if(pvState == "off" && evState == "off" && hpState == "On" && battState == "Off") {

    power = usage_profile_noev_base[currentHour]; // Get the power consumption for the current hour

    HV1_animation_enabled = true;
    HV2_animation_enabled = true;
    HV3_animation_enabled = true;
    lv1_animation_enabled = true;
    lv2_animation_enabled = true;
    batt_animation_enabled = false;

    Serial.print("Current Hour: " + String(currentHour));
    Serial.print("  ");
    Serial.println("Current load: " + String(power) + "W");
    displayUART('P', power);
    displayUART('H', currentHour);

    animationspeedMap(power);
  }

  //  Usage profile with HP, no EV, with PV
  if(pvState == "On" && evState == "off" && hpState == "On" && battState == "Off") {

    power = usage_profile_noev_with_PV[currentHour]; // Get the power consumption for the current hour

    HV1_animation_enabled = true;
    HV2_animation_enabled = true;
    HV3_animation_enabled = true;
    lv1_animation_enabled = true;
    lv2_animation_enabled = true;
    batt_animation_enabled = false;

    Serial.print("Current Hour: " + String(currentHour));
    Serial.print("  ");
    Serial.println("Current load: " + String(power) + "W");
    displayUART('P', power);
    displayUART('H', currentHour);

    animationspeedMap(power);
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
      json += "\"27\":\"" + output27State + "\",";
      json += "\"pvState\":\"" + pvState + "\",";
      json += "\"windState\":\"" + windState + "\",";
      json += "\"evState\":\"" + evState + "\",";
      json += "\"hpState\":\"" + hpState + "\",";
      json += "\"battState\":\"" + battState + "\",";
      json += "\"hourValue\":" + String(currentHour);
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

  // Handle load control HTTPS requests
  server.on("/pv/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    pvState = "off";
    Serial.println("PV state set to off");
    request->send(200, "text/plain", "OK");
  });

  server.on("/pv/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    pvState = "On";
    Serial.println("PV state set to On");
    request->send(200, "text/plain", "OK");
  });

  server.on("/wind/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    windState = "off";
    Serial.println("Wind state set to off");
    request->send(200, "text/plain", "OK");
  });

  server.on("/wind/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    windState = "On";
    Serial.println("Wind state set to On");
    request->send(200, "text/plain", "OK");
  });

  server.on("/ev/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    evState = "off";
    battState = "off";
    Serial.println("EV state set to off");
    request->send(200, "text/plain", "OK");
  });

  server.on("/ev/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    evState = "On";
    Serial.println("EV state set to On");
    request->send(200, "text/plain", "OK");
  });

  server.on("/hp/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    hpState = "off";
    battState = "off";
    Serial.println("HP state set to off");
    request->send(200, "text/plain", "OK");
  });

  server.on("/hp/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    hpState = "On";
    Serial.println("HP state set to On");
    request->send(200, "text/plain", "OK");
  });

  server.on("/batt/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    battState = "off";
    Serial.println("Battery state set to off");
    request->send(200, "text/plain", "OK");
  });

  server.on("/batt/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(hpState == "On" && evState == "On") {
      battState = "On";
      Serial.println("Battery state set to on");
    }
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