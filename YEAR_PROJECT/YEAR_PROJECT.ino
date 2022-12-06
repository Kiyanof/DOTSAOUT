#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HardwareSerial.h>


HardwareSerial SerialPort2(2);

// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 100;

// Replace with your network credentials
const char* ssid = "Escapemoon";
const char* password = "Mkcm0936@13777731";

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 109);
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
    body {  margin: 0;height: 100%; background-color: black;}
    .topnav { overflow: hidden; background-color: #000; color: #E11616; font-size: 1rem; }
    .content { padding: 20px; border: 1px solid #E11616;} //#2cc12e GREEN  #2616E1 blue
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 900px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>7 SEGMENT</h1>
  </div>
  <div class="content">
    <div class="cards">
      <img id="segment1" src="zero.png" width="200" height="300" alt="">
      <img id="segment2" src="zero.png" width="200" height="300" alt="">
      <img id="segment3" src="zero.png" width="200" height="300" alt="">
      <img id="segment4" src="zero.png" width="200" height="300" alt="">
    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');

 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);

 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);

 for (var i = 0; i < 4; i++) {
   source.addEventListener('segment' + i, function(e) {
    console.log("segment 1: ", e.data);
    ls = ["zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"]
    for (var i = 0; i < ls.length; i++) {
      if (e.data == i) {
        document.getElementById("segment" + i).src=ls[i] + ".png";
        break;
      }
    }

  }, false);
 }

 source.addEventListener('mode', function(e) {
  console.log("mode: ", e.data);
  switch (e.data) {
    case 0:
      document.getElementById('content').style.border = "1px solid #E11616;";
      break;
      case 1:
        document.getElementById('content').style.border = "1px solid #2cc12e;";
        break;
        case 2:
          document.getElementById('content').style.border = "1px solid #2616E1;";
          break;
    default: document.getElementById('content').style.border = "1px solid #E11616;";

  }
 }, false);

}
</script>
</body>
</html>)rawliteral";

int segment[4] = {0,0,0,0};
int _mode = 0;

void setSegmentNumber(int num){
  if(num >= 0){
    int number = num;
    for(int i = 0; i < 4; i++){
      segment[3 - i] = number % 10;
      number /= 10;     
    }
  } else if(num == -1) _mode = 0;
    else if(num == -2) _mode = 1;
    else if(num == -3) _mode = 2;
    else return;
}

String processor(const String& var){
  //Serial.println(var);
  if(var == "segment1"){
    return String(segment[0]);
  }
  else if(var == "segment2"){
    return String(segment[1]);
  }
  else if(var == "segment3"){
    return String(segment[2]);
  }
  else if(var == "segment4"){
    return String(segment[3]);
  }

  else if(var == "mode"){
    return String(_mode);
  }

  return String();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SerialPort2.begin(15200, SERIAL_8N1, 16, 17);

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
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (SerialPort2.available())
  {
    int number = SerialPort2.read();
    setSegmentNumber(number);
    
  }


  if ((millis() - lastTime) > timerDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send("ping",NULL,millis());
    events.send(String(segment[1-1]).c_str(),"segment1",millis());
    events.send(String(segment[2-1]).c_str(),"segment2",millis());
    events.send(String(segment[3-1]).c_str(),"segment3",millis());
    events.send(String(segment[4-1]).c_str(),"segment4",millis());

    events.send(String(_mode).c_str(),"mode",millis());
   
    lastTime = millis();
    }
}
