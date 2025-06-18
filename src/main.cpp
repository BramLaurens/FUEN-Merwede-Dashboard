#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <FastLED.h>

#define STRIP_PIN 4 
#define NUM_LEDS 144
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS  100

CRGB leds[NUM_LEDS];

// Network credentials
const char* ssid     = "FUEN-EV2A-1";
const char* password = "123456789";

// Server port
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Buttonstates
String output2State = "off";
String output27State = "off";
String runState = "Stopped";

// Assign output variables to GPIO pins
const int output2 = 2; 
const int output27 = 27;

// Prototypes
void HTML_handler();

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

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();

  FastLED.addLeds<LED_TYPE, STRIP_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // Set all LEDs to a static color (e.g., red)
  fill_solid(leds, NUM_LEDS, CRGB::Blue);  // Options: Red, Green, Blue, etc.
  FastLED.show();  // Send data to the strip
}

void loop() {
  HTML_handler();
}

void HTML_handler() {
 WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    header = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {

            // --- Handle GPIO commands ---
            if (header.indexOf("GET /2/on") >= 0) {
              output2State = "on";
              digitalWrite(output2, HIGH);
            } else if (header.indexOf("GET /2/off") >= 0) {
              output2State = "off";
              digitalWrite(output2, LOW);
            } 

            else if (header.indexOf("GET /run_state/run") >= 0) {
              runState = "Running";
            }

            else if (header.indexOf("GET /run_state/stop") >= 0) {
             runState = "Stopped";
            }

            else if (header.indexOf("GET /27/on") >= 0) {
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              output27State = "off";
              digitalWrite(output27, LOW);
            }

            // --- Serve JSON with GPIO states ---
            if (header.indexOf("GET /gpio") >= 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json");
              client.println("Connection: close");
              client.println();

              String json = "{";
              json += "\"runState\":\"" + runState + "\",";
              json += "\"2\":\"" + output2State + "\",";
              json += "\"27\":\"" + output27State + "\"";
              json += "}";

              client.print(json);
            }

            // --- Serve style.css ---
            else if (header.indexOf("GET /style.css") >= 0) {
              File file = LittleFS.open("/style.css", "r");
              if (file) {
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/css");
                client.println("Connection: close");
                client.println();
                while (file.available()) {
                  client.write(file.read());
                }
                file.close();
              }

            // --- Serve index.html ---
            } else if (header.indexOf("GET /") >= 0) {
              File file = LittleFS.open("/index.html", "r");
              if (file) {
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/html");
                client.println("Connection: close");
                client.println();
                while (file.available()) {
                  client.write(file.read());
                }
                file.close();
              }
            }

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";
    client.stop();
    Serial.println("Client disconnected.");
  }
}