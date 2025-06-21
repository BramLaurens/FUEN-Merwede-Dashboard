#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <FastLED.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <HardwareSerial.h>

// Hardware Serial object
HardwareSerial SerialUART(2); // Use UART2 for communication

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

void setup() {
  Serial.begin(115200);
  SerialUART.begin(115200, SERIAL_8N1, 26, 27); // Initialize UART2 with RX on GPIO 26 and TX on GPIO 27

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

}

void loop() {
  if(SerialUART.available()) {
    String command = SerialUART.readStringUntil('\n');
    Serial.println("Received command: " + command);

    if (command == "FILL"){
      display.clearDisplay();
      display2.clearDisplay();
      display.fillScreen(SSD1306_WHITE);
    }
  }
}