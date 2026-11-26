// Mini 11by 10 led strip display board code
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_GFX.h>
const char* ssid = "ESP8266_Message";
const char* password = "password123";
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
#define PIN D4
#define WIDTH 11
#define HEIGHT 10
#define ACTIVE_HEIGHT 5 // Use only the top 5 lines
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(WIDTH, HEIGHT, PIN, NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);
String currentMessage = "";
int x = matrix.width();
// Define a 5x5 font for letters A-Z
const uint8_t font5x5[][5] = {
    {0x0E, 0x11, 0x1F, 0x11, 0x11}, // A
    {0x1E, 0x11, 0x1E, 0x11, 0x1E}, // B
    {0x1E, 0x10, 0x10, 0x10, 0x1E}, // C
    {0x1C, 0x12, 0x11, 0x12, 0x1C}, // D
    {0x1F, 0x10, 0x1E, 0x10, 0x1F}, // E
    {0x1F, 0x10, 0x1E, 0x10, 0x10}, // F
    {0x0E, 0x10, 0x17, 0x11, 0x0E}, // G
    {0x11, 0x11, 0x1F, 0x11, 0x11}, // H
    {0x1F, 0x04, 0x04, 0x04, 0x1F}, // I
    {0x1F, 0x02, 0x02, 0x12, 0x1E}, // J
    {0x11, 0x12, 0x1C, 0x12, 0x11}, // K
    {0x10, 0x10, 0x10, 0x10, 0x1F}, // L
    {0x11, 0x1B, 0x15, 0x11, 0x11}, // M
    {0x11, 0x19, 0x15, 0x13, 0x11}, // N
    {0x0E, 0x11, 0x11, 0x11, 0x0E}, // O
    {0x1E, 0x11, 0x1E, 0x10, 0x10}, // P
    {0x0E, 0x11, 0x11, 0x15, 0x0E}, // Q
    {0x1E, 0x11, 0x1E, 0x12, 0x11}, // R
    {0x1F, 0x10, 0x1E, 0x01, 0x1F}, // S
    {0x1F, 0x04, 0x04, 0x04, 0x04}, // T
    {0x11, 0x11, 0x11, 0x11, 0x0E}, // U
    {0x11, 0x11, 0x11, 0x0A, 0x04}, // V
    {0x11, 0x11, 0x15, 0x1B, 0x11}, // W
    {0x11, 0x0A, 0x04, 0x0A, 0x11}, // X
    {0x11, 0x11, 0x0A, 0x04, 0x04}, // Y
    {0x1F, 0x02, 0x04, 0x08, 0x1F}, // Z
};
// Draw a character from the 5x5 font onto the matrix
void drawChar(int16_t x, int16_t y, char c) {
  if (c < 'A' || c > 'Z') return; // Only handle 'A' to 'Z'
  const uint8_t *bitmap = font5x5[c - 'A'];
  for (int i = 0; i < 5; i++) {    // Loop through rows
    uint8_t line = bitmap[i];
    for (int j = 0; j < 5; j++) {  // Loop through columns
      if (line & (1 << (4 - j))) { // Check pixel bit
        matrix.drawPixel(x + j, y + i, matrix.Color(0, 255, 0)); // Draw pixel
      }
    }
  }
}
// Render and scroll the current message
void scrollMessage() {
  matrix.fillScreen(0); // Clear the screen
  int16_t posX = x;
  for (char c : currentMessage) {  // Loop through each character in the message
    drawChar(posX, 0, c);          // Draw character at the current position
    posX += 6;                     // Move to the next character (5 pixels + 1 space)
  }
  if (--x < -((int)currentMessage.length() * 6)) {
    x = matrix.width();            // Reset scroll position
  }
  matrix.show();
  delay(100); // Adjust scrolling speed
}
void displayMessage(String message) {
  currentMessage = message;
  x = matrix.width(); // Reset scroll position
}
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
                      AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("WebSocket client connected");
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("WebSocket client disconnected");
  } else if (type == WS_EVT_DATA) {
    data[len] = 0;
    String message = (char*)data;
    Serial.println("Message received: " + message);
    displayMessage(message);
    client->text("Message received: " + message);
  }
}
void setup() {
  Serial.begin(115200);
  matrix.begin();
  matrix.setTextWrap(false); // Disable automatic text wrapping
  matrix.setBrightness(40); // Adjust brightness
  matrix.setTextColor(matrix.Color(0, 255, 0)); // Green text
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  dnsServer.start(53, "*", apIP);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP8266 LED Matrix Message Display</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            flex-direction: column;
            height: 100vh;
            background-color: #f4f4f4;
        }
        h1 {
            color: #333;
        }
        input, button {
            padding: 10px;
            font-size: 16px;
            margin: 5px;
            border-radius: 5px;
        }
        button {
            background-color: #007bff;
            color: white;
            border: none;
            cursor: pointer;
        }
        button:hover {
            background-color: #0056b3;
        }
        #status {
            margin-top: 20px;
            font-size: 18px;
        }
    </style>
</head>
<body>
    <h1>ESP8266 LED Matrix Message Display</h1>
    <input type="text" id="messageInput" placeholder="Enter message">
    <button id="sendButton">Send Message</button>
    <div id="status">Status: Waiting for message</div>
    <script>
        let socket = new WebSocket('ws://' + location.hostname + '/ws');
        socket.onopen = function(e) {
          console.log("[open] Connection established");
        };
        socket.onmessage = function(event) {
          console.log(`[message] Data received from server: ${event.data}`);
          document.getElementById('status').innerText = event.data;
        };
        socket.onclose = function(event) {
          if (event.wasClean) {
            console.log(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
          } else {
            console.log('[close] Connection died');
          }
        };
        socket.onerror = function(error) {
          console.error(`[error] ${error.message}`);
        };
        document.getElementById('sendButton').addEventListener('click', function() {
            let message = document.getElementById('messageInput').value;
            socket.send(message);
        });
    </script>
</body>
</html>
)rawliteral");
  });
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "404: Not Found");
  });
  server.begin();
  Serial.println("Web server and WebSocket started");
}
void loop() {
  dnsServer.processNextRequest();
  ws.cleanupClients();
  scrollMessage();
}

