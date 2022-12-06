#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

int a = 0;
// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 1000;

// Replace with your network credentials
const char* ssid = "iPhone";
const char* password = "seti5371";

// Set your Static IP address
IPAddress local_IP(172, 20, 10, 3);
// Set your Gateway IP address
IPAddress gateway(172, 20, 10, 1);

IPAddress subnet(255, 255, 255, 240);

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
    <h1>S P E E D</h1>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card">
        <p><i class="fa fa-info" style="color:#059e8a;"></i> VOLUME </p><p><span class="reading"><span id="volume">%VOLUME%</span></span></p>
      </div>
    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('volume', function(e) {
  console.log("volume", e.data);
  document.getElementById("volume").innerHTML = e.data;
 }, false);

}
</script>
</body>
</html>)rawliteral";

String processor(const String& var){
  //Serial.println(var);
  if(var=="VOLUME") return String(a);
  return String();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
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
  

  if ((millis() - lastTime) > timerDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(String(a++).c_str(),"volume",millis());
   if (a == 11) a = 0;
    lastTime = millis();
    }
}
