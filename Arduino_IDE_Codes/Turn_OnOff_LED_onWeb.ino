// Web server LED on off switch
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
const char* ssid = "ESP8266_Control";
const char* password = "password123";
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
const int ledPin = D4;
bool ledState = false;

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
                      AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("WebSocket client connected");
    client->text(ledState ? "ON" : "OFF");
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("WebSocket client disconnected");
  } else if (type == WS_EVT_DATA) {
    data[len] = 0;
    String message = (char*)data;
    Serial.println("Message received: " + message);
    if (message == "ON") {
      digitalWrite(ledPin, HIGH);
      ledState = true;
    } else if (message == "OFF") {
      digitalWrite(ledPin, LOW);
      ledState = false;
    }
    client->text(ledState ? "ON" : "OFF");
  }
}
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
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
    <title>ESP8266 LED Control</title>
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
        button {
            padding: 10px 20px;
            font-size: 16px;
            cursor: pointer;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 5px;
            margin: 5px;
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
    <h1>ESP8266 LED Control</h1>
    <button id="onButton">Turn LED ON</button>
    <button id="offButton">Turn LED OFF</button>
    <div id="status">Status: OFF</div>
    <script>
        let socket = new WebSocket('ws://' + location.hostname + '/ws');
        socket.onopen = function(e) {
          console.log("[open] Connection established");
        };
        socket.onmessage = function(event) {
          console.log(`[message] Data received from server: ${event.data}`);
          document.getElementById('status').innerText = "Status: " + event.data;
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
        document.getElementById('onButton').addEventListener('click', function() {
            socket.send('ON');
        });
        document.getElementById('offButton').addEventListener('click', function() {
            socket.send('OFF');
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
}



