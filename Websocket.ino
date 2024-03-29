#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

//WiFi Configuration
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

//Relay Pin
const int RELAY_PIN = 36;

//PIR Sensor
const int PIR_SENSOR_PIN = 16; 

//Door Sensor
const int DOOR_SENSOR_PIN = 18;

//Websocket Properties
AsyncWebServer server(80);
AsyncWebSocket webSocket("/ws");

//Relay Status
int relayState = LOW;

//PIR Status
int pirState = LOW;

//Door Status
int doorState = LOW;

//Time Configurations for Sending Sensor Status
int countTimeWas = millis();
long countVal = 0;

//Websocket Handler
void handleWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len){
  switch(type){
    case WS_EVT_DISCONNECT:
      Serial.printf("[%u] Disconnected from the server!\n", client->id());
      break;
    case WS_EVT_CONNECT:
      {
        IPAddress ip = client->remoteIP();
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", client->id(), ip[0], ip[1], ip[2], ip[3]);
        String relayStatusUpdate = "{\"label\":\"relayStatus\",\"value\":\"" + String(relayState? "RELAY ON" : "RELAY OFF") + "\"}";
        client->text(relayStatusUpdate);
      }
      break;
    case WS_EVT_DATA:
      {
        AwsFrameInfo* info = (AwsFrameInfo*)arg;
        if(info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT){
          data[len] = '\0';
          String payload = String ((char*)data);

          Serial.printf("[%u] Received: %s\n", client->id(), payload.c_str());

          if(payload == "toggle"){
            relayState = !relayState;
            digitalWrite(RELAY_PIN, !relayState);

            String relayUpdate = "{\"label\":\"relayStatus\",\"value\":\"" + String(relayState ? "RELAY ON" : "RELAY OFF") + "\"}";

            webSocket.textAll(relayUpdate);
          }
        }
      }

      break;

  }
}

//Root Handler
void handleRoot(AsyncWebServerRequest* request){
  request->send(200, "text/html", R"HTML(
    <!DOCTYPE html>
    <html>
      <head>
        <meta name='viewport' content='width=device-width, initial-scale=1.0'>
        <title>Websocket ESP32</title>
      </head>

      <body>
        <h1>ESP32 Websocket Client</h1>
        <hr>
        <h2>Relay Information</h2>
        <button onclick='toggleRelay()'>Toggle Relay</button>
        <p>Relay Status : <span id='relayStatus'></span></p>
        <hr>
        <h2>Sensor Information</h2>
        <p>Sensor PIR Status : <span id='pirData'></span></p>
        <p>Sensor Door Status : <span id='doorData'></span></p>
        <p><span id='message'></span></p>
        <br clear='all'>

        <script>
          var webSocket;
          connectWebSocket();
          
          function connectWebSocket(){
            webSocket = new WebSocket('ws://' + window.location.hostname + '/ws');
            
            webSocket.onopen = function(event){
              console.log('WebSocket connected');
            };

            webSocket.onmessage = function(event){
              var data = JSON.parse(event.data);
              if(data.label == 'relayStatus'){
                document.getElementById('relayStatus').innerHTML = data.value;
              } else if(data.label == 'pirData'){
                document.getElementById('pirData').innerHTML = data.value;
              } else if(data.label = 'doorData'){
                document.getElementById('doorData').innerHTML = data.value;
              }
            };

            webSocket.onclose = function(event){
              console.log('WebSocket disconnected');
              var messageElement = document.getElementById('message');
              messageElement.innerHTML ='WebSocket disconnected. Reconnecting...';
              setTimeout(connectWebSocket, 2000);
              setTimeout(function() { messageElement.innerHTML = ''; }, 3000);
            };

            webSocket.onerror = function(event){
              console.error('WebSocket error:', event);
            };
          }

          function toggleRelay(){
            webSocket.send('toggle');
          }
        </script>
      </body>
    </html>
  )HTML");
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connection Established");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Start to use WebSockets");

  server.on("/", HTTP_GET, handleRoot);

  server.addHandler(&webSocket);
  webSocket.onEvent(handleWebSocketEvent);

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  pirState = digitalRead(PIR_SENSOR_PIN);
  doorState = digitalRead(DOOR_SENSOR_PIN);

  if((pirState == LOW) && (doorState == HIGH)){
    relayState = HIGH;
    digitalWrite(RELAY_PIN, relayState);
    String relayUpdate = "{\"label\":\"relayStatus\",\"value\":\"" + String(relayState ? "RELAY ON" : "RELAY OFF") + "\"}";
    webSocket.textAll(relayUpdate);
  } else if((pirState == HIGH) && (doorState == LOW)){
    relayState = LOW;
    digitalWrite(RELAY_PIN, relayState);
    String relayUpdate = "{\"label\":\"relayStatus\",\"value\":\"" + String(relayState ? "RELAY ON" : "RELAY OFF") + "\"}";
    webSocket.textAll(relayUpdate);
  }


  if(millis() - countTimeWas >= 1000){
    countTimeWas = millis();

    String doorUpdate = "{\"label\":\"doorData\",\"value\":" + String(doorState) + "}";
    String pirUpdate = "{\"label\":\"pirData\",\"value\":" + String(pirState) + "}";

    webSocket.textAll(doorUpdate);
    webSocket.textAll(pirUpdate);
  }

}
