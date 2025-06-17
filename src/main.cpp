#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>

/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Replace with your network credentials
const char* ssid     = "FUEN-EV2A-1";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
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
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    header = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;

        if (c == '\n') {
          // End of request
          if (currentLine.length() == 0) {

            // Handle GPIO requests
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 ON");
              output26State = "on";
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 OFF");
              output26State = "off";
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 ON");
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 OFF");
              output27State = "off";
              digitalWrite(output27, LOW);
            }

            // Serve CSS
            if (header.indexOf("GET /style.css") >= 0) {
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

            // Serve HTML and inject states
            } else {
              File file = LittleFS.open("/index.html", "r");
              if (file) {
                String html = file.readString();
                file.close();

                // Replace placeholders with actual GPIO state
                html.replace("{{STATE26}}", output26State);
                html.replace("{{STATE27}}", output27State);

                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/html");
                client.println("Connection: close");
                client.println();
                client.println(html);
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

    // End of connection
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println();
  }
}
