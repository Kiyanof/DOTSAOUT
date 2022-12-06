#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "dotsaout";
const char* password = "v123456789st";

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 41);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 0);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

// Initialize WiFi
void initWiFi() {
    WiFi.mode(WIFI_STA);
    if (!WiFi.config(local_IP, gateway, subnet)) {
      Serial.println("STA Failed to configure");
    }
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #50B8B4; color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>T I M E G A D J E T</h1>
  </div>
}
</script>
</body>
</html>)rawliteral";

String processor(const String& var){
  return String();
}
//3600 - 3700 green +5MIN
//2800 - 2900 yellow +3MIN

#define GREEN_MIN 1300
#define GREEN_MAX 1400


int green[4] = {34, 35, 32, 33};
int value = 0;

int greenVal[4] = {0, 0, 0, 0};
int prevGreenVal[4] = {0, 0, 0, 0};

void setup() {
  Serial.begin(9600);

  for(int i = 0; i < 4; i++){
    pinMode(green[i], INPUT_PULLUP);
  }
  initWiFi();

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.begin();
}

void loop() {
  //events.send(String(st).c_str(),"volume",millis());
  for(int i = 0; i < 4; i++){
    value = analogRead(green[i]);
    Serial.print(i+1);
    Serial.print("G:");
    Serial.println(value);
    if (value > GREEN_MIN && value < GREEN_MAX) greenVal[i] = 1;
    else greenVal[i] = 0;

    if(greenVal[i] != prevGreenVal[i]){
      Serial.print(i);
      Serial.print("G: ");
      Serial.println("change");
      events.send(String(greenVal[i]).c_str(),String(i+1).c_str(),millis());
      prevGreenVal[i] = greenVal[i];
    }
  }
delay(3000);  


  
}
