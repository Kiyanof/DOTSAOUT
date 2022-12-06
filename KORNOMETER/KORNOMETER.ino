#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>


#define BCD3 23
#define BCD2 22
#define BCD1 21
#define BCD0 19

#define LE3 26
#define LE2 14
#define LE1 12
#define LE0 13

// the number that will get displayed
int timer = 0;
int minute = 0;
int second = 0;

/*
 * The parameters of the SegmentDisplay_CD4511B constructor:
 *    First 4 numbers are the Arduino pins connected to the  A, B, C, D pins of the CD4511B chip(s)
 *      in this example 6,8,7,5 are the pins used
 *    5-th number is the number of chips used 
 *      in this example 2 chips are used to drive 2 displays
 *    the remaining pins are the Arduino pins connected to the latching pins of the CD4511B chip(s) 
 *      in this example 9 and 10 are the numbers of the latching pins
*/

// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 1000;

// Replace with your network credentials
const char* ssid = "dotsaout";
const char* password = "v123456789st";

const char* PARAM_INPUT_1 = "minute";  
const char* PARAM_INPUT_2 = "second";
const char* PARAM_INPUT_3 = "start";

int decrease(int inp){
  if(inp > 0) return inp - 1;
  else return 60;
}

bool play = false;

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 24);
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
  <title>KORNOMETEr</title>
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
    .cards { max-width: 900px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); }
    .reading { font-size: 1.4rem; }
    input {min-width: 200px;}
    button {
      border-radius: 6px;
      background-color: transparent;
      min-width: 120px;
      min-height: 40px;
      color: green;
      border-color: green;}
      button:hover {
        background-color: green;
        color: white;

      }
      form {
        color:green;
        border: 1px solid #30b325;
        padding: 22px;
        margin-top: 12px;
        box-shadow: 12px 12px rgba(127, 247, 225, 0.2);
      }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>timer</h1>
  </div>
  <div class="content">
    <div class="cards">
      <img id="segment1" src="zero.png" width="150" height="250" alt="">
      <img id="segment2" src="zero.png" width="150" height="250" alt="">
      <img src="dot.png" alt="">
      <img id="segment3" src="zero.png" width="150" height="250" alt="">
      <img id="segment4" src="zero.png" width="150" height="250" alt="">
    </div>
  </div>
  <br />
    <form class="">
      <caption>S E T T I N G</caption>
      <hr style="background-color:white;" />
        <h4>minute: </h4><label id="minuteRangeLabel"></label><h5>
        <input id="minuteRange"  oninput="document.getElementById('minuteRangeLabel').innerHTML = this.value;" value="0" type="range" min="0" max="60" step="1">
      </input></h5>
      <h4>second: </h4><label id="secondRangeLabel"></label><h5>
        <span id="second">
          <input id="secondRange"  oninput="document.getElementById('secondRangeLabel').innerHTML = this.value;" value="0" type="range" min="0" max="60" step="1">
        </input></span></h5>
    <button type="button" name="button" onclick="conf()">UPDATE</button>
    <button type="button" name="button" onclick="start()">START</button>
    <button type="button" name="button" onclick="stop()">STOP</button>
    </form>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');

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

 source.addEventListener('minute', function(e) {
   console.log("minute" + e.data);
  }, false);
source.addEventListener('second', function(e) {
    console.log("second" + e.data)
   }, false);

}
</script>
<script>function conf() {
  var minute = document.getElementById('minuteRange').value;
  var second = document.getElementById('secondRange').value;
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/update?minute="+minute+"&second="+second, true);
  xhr.send();
}</script>
<script>function start() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/update?start=1", true);
  xhr.send();
}</script>
<script>function stop() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/update?start=0", true);
  xhr.send();
}</script>


</body>
</html>
)rawliteral";

int segment[4] = {0,0,0,0};
void setSegmentNumber(){
  int num = minute * 100 + second;
  if(num >= 0){
    int number = num;
    for(int i = 0; i < 4; i++){
      segment[3 - i] = number % 10;
      number /= 10;     
    }
  } 
}

int BCDS[4]  = {BCD0, BCD1, BCD2, BCD3};
int LATCH[4] = {LE0, LE1, LE2, LE3};

uint8_t BCD_NUMBER[10][4] = {
                   {LOW, LOW, LOW, LOW},
                   {LOW, LOW, LOW, HIGH},
                   {LOW, LOW, HIGH, LOW},
                   {LOW, LOW, HIGH, HIGH},
                   {LOW, HIGH, LOW, LOW},
                   {LOW, HIGH, LOW, HIGH},
                   {LOW, HIGH, HIGH, LOW},
                   {LOW, HIGH, HIGH, HIGH},
                   {HIGH, LOW, LOW, LOW},
                   {HIGH, LOW, LOW, HIGH}
};

void changeBCD(int num){
  for(int i = 0; i < 4; i++){
    digitalWrite(BCDS[i], BCD_NUMBER[num][i]); 
  }
}

void showNumber(){
  for(int i = 0; i < 4; i++){
    digitalWrite(LATCH[i], LOW);
      changeBCD(segment[i]);  
    digitalWrite(LATCH[i], HIGH);
  }
}

void setup() {
  Serial.begin(9600);
  for(int i = 0; i < 4; i++){
    pinMode(BCDS[i], OUTPUT);
    digitalWrite(BCDS[i], LOW);
  }

  for(int i = 0; i < 4; i++){
    pinMode(LATCH[i], OUTPUT);
    digitalWrite(LATCH[i], LOW);
  }
  
  initWiFi();

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/update?relay=<inputMessage>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    String inputMessage2;
    String inputParam2;
    // GET input1 value on <ESP_IP>/update?relay=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1) & request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
      minute = inputMessage.toInt();
      second = inputMessage2.toInt();
    }
    else if (request->hasParam(PARAM_INPUT_3)){
      Serial.println("REQ2");
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;
      play = inputMessage.toInt();
      Serial.println(inputMessage);
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage + inputMessage2);
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

  if(play && ((millis() - lastTime) > timerDelay)){
    second = decrease(second);    
    if(second == 60){
      second = decrease(second);  
      minute = decrease(minute);
    }
    if(minute == 0 && second == 0) play = false; 

    setSegmentNumber();

    events.send(String(segment[1-1]).c_str(),"segment1",millis());
    events.send(String(segment[2-1]).c_str(),"segment2",millis());
    events.send(String(segment[3-1]).c_str(),"segment3",millis());
    events.send(String(segment[4-1]).c_str(),"segment4",millis());
    events.send(String(minute).c_str(),"minute",millis());
    events.send(String(second).c_str(),"second",millis());

    lastTime = millis();
  }
  timer = minute * 100 + second;
  showNumber();    
  
}
