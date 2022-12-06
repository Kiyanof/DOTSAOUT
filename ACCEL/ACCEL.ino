#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#define INPUTA 35
#define INPUTB 34


#define STEP 819
int volume[10] = {23, 22, 21, 19, 18, 33, 25, 26, 27, 14};//4095//5 = 819

int a = analogRead(INPUTA);
int b = analogRead(INPUTB);
int sum = a + b;
int st  = 0;
int prevSt = 0;
int mode = 0;

// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 1000;

// Replace with your network credentials
const char* ssid = "dotsaout";
const char* password = "v123456789st";

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 29);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 0);

const char* PARAM_INPUT_1 = "mode";  

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
    <h1>A C C E L</h1>
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
  if(var=="VOLUME") return String(st);
  return String();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  initWiFi();
  for(int i = 0; i < 10; i ++){
    pinMode(volume[i], OUTPUT);
    digitalWrite(volume[i], LOW);
  }

  

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?relay=<inputMessage>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/update?relay=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      Serial.println(inputMessage);
      mode = inputMessage.toInt();
    }
    request->send(200, "text/plain", "OK");
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
  // put your main code here, to run repeatedly:
  a = analogRead(INPUTA);
  b = analogRead(INPUTB);  
  sum = a + b;
  st = sum / 819;
  Serial.println(a);
  if(mode == 1){
    for(int i = 0; i < st; i++){
      digitalWrite(volume[i], HIGH);    
    }
    for(int i = st; i < 10; i++){
      digitalWrite(volume[i], LOW);    
    }
  }
  else if (mode == 0){
    for(int i = st; i < 10; i++){
      digitalWrite(volume[i], LOW);    
    }
    delay(50);
    for(int i = 0; i < st; i++){
      digitalWrite(volume[i], HIGH);    
    }
    delay(50);
    for(int i = 0; i < st; i++){
      digitalWrite(volume[i], LOW);    
    } 
  }
else  {
  for(int i = 0; i < 10; i++){
    digitalWrite(volume[i], HIGH); 
    delay(25);   
  }  

  for(int i = 9; i >= 0; i--){
    digitalWrite(volume[i], LOW); 
    delay(25); 
  }
}

  if (st != prevSt) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(String(st).c_str(),"volume",millis());
    prevSt = st;    
  }
}