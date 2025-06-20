#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <FastLED.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <SPI.h>

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

// Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// #define OLED_MOSI   22
// #define OLED_CLK   18
#define OLED_DC    16
#define OLED_CS    5
#define OLED_RESET 17

#define OLED_CS2   4
#define OLED_RESET2 21

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET2, OLED_CS2);


// Prototypes
void HTML_handler();
void stripBlue();
void stripOff();
void HV1_animation();
void HV2_animation();
void HV3_animation();

void setup() {
  Serial.begin(115200);
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

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }

  if (!display2.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("Display 2 allocation failed"));
  }

  Serial.println(F("Initialized screens!"));

  display.display();
  display2.display();

  delay(500);

  // Clear the buffer
  display.clearDisplay();
  display.display();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.setTextSize(1);
  display.print(F("Merwede"));
  display.setCursor(10, 50);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.setTextSize(1);
  display.print(F("Merwede"));

  display2.clearDisplay();
  display2.display();
  display2.setTextColor(SSD1306_WHITE);
  display2.setCursor(10, 20);
  display2.setTextSize(1);
  display2.print(F("Merwede"));
  display2.setCursor(10, 50);
  display2.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display2.setTextSize(1);
  display2.print(F("Merwede"));
 
  // Refresh (apply command)
  display.display();
  display2.display();


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

  HTML_handler();

}

void loop() {

  if(runState == "Running") {
    HV1_animation();  // Run the animation function
    HV2_animation();  // Run the animation function
    HV3_animation();  // Run the animation function
    digitalWrite(houseleds_pin, HIGH);  // Turn on the house LEDs
  }
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