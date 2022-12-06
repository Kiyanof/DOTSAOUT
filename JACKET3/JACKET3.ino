#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <MFRC522.h>

/*
 * STOP WATCH -> WHITE -> 2
 * GAURD      -> RED   -> 2
 * SWITCHER   -> PURPLE -> 2
 * WISE       -> YELLOW -> 2 
 * GADJET     -> GREEN  -> RFID NADARE IN
 * ROBO TEK   -> BLUE   -> 2
 * 
 * 
 */

// PIN Numbers : RESET + SDAs
#define RST_PIN       D3 //TODO: CHANGE PIN TO NODE MCU
#define SELECT        D1 
#define SKILL_RELAY   9
#define DAMAGE_RELAY  10


byte UIDS[2][4] = { {0x4A, 0xD0, 0x72, 0xBE},
                    {0x9C, 0xF8, 0x17, 0x4A}
};

// Create an MFRC522 instance :
MFRC522 mfrc522;

#define counter 2
bool STOP_WATCH = false;

// Set to true to define Relay as Normally Open (NO)
#define RELAY_NO    false

// Set number of relays
#define NUM_RELAYS  2

// Assign each GPIO to a relay
int relayGPIOs[NUM_RELAYS] = {SKILL_RELAY, DAMAGE_RELAY};

// Timer variables
unsigned long lastTime = 0;  
unsigned long lastTime2 = 0;
unsigned long timerDelay = 1000;
unsigned long timerDelay2 = 500;

// Replace with your network credentials
const char* ssid = "dotsaout";
const char* password = "v123456789st";

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 33);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 0);

const char* PARAM_INPUT_1 = "relay";  
const char* PARAM_INPUT_2 = "state";
const char* PARAM_INPUT_3 = "skill";
const char* PARAM_INPUT_4 = "timer";

int skill = 2;
int timer = 30;
int timing = timer;

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
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px}
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
    input {min-width: 200px;}
    button {
      border-radius: 6px;
      background-color: transparent;
      min-width: 120px;
      min-height: 40px;
      color: purple;
      border-color: purple;}
      button:hover {
        background-color: purple;
        color: white;

      }
      form {
        border: 1px solid #30b325;
        padding: 22px;
        margin-top: 12px;
        box-shadow: 12px 12px rgba(127, 247, 225, 0.2);
      }
  </style>
</head>
<body>
  <h2>C O R O M</h2>
  %BUTTONPLACEHOLDER%
  <h4>SKILL: </h4><span id="skill"></span>
  <h4>TIMER: </h4><span id="timer"></span>
  <br />
    <form class="">
      <caption>S E T T I N G</caption>
      <hr />
        <h4>SKILL REMAINING: </h4><label id="skillRangeLabel"></label><h5>
        <input id="skillRange"  oninput="document.getElementById('skillRangeLabel').innerHTML = this.value;" value="0" type="range" min="0" max="10" step="1">
      </input></h5>
      <h4>TIMER: </h4><label id="timerRangeLabel"></label><h5>
        <span id="timer">
          <input id="timerRange"  oninput="document.getElementById('timerRangeLabel').innerHTML = this.value;" value="0" type="range" min="0" max="60" step="1">
        </input></span></h5>
    <button type="button" name="button" onclick="conf()">UPDATE</button>
    </form>


<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?relay="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?relay="+element.id+"&state=0", true); }
  xhr.send();
}</script>
<script>function conf() {
  var skill = document.getElementById('skillRange').value;
  var timer = document.getElementById('timerRange').value;
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/update?skill="+skill+"&timer="+timer, true);
  xhr.send();
}</script>
<script>
  if (!!window.EventSource) {
    var source = new EventSource('/events');

    source.addEventListener('skill', function(e) {
      console.log("skill: ", e.data);
      document.getElementById("skill").innerHTML = e.data;
     }, false);

   source.addEventListener('timer', function(e) {
      console.log("timer: ", e.data);
      document.getElementById("timer").innerHTML = e.data;
     }, false);
  }
 </script>
</body>
</html>

  

)rawliteral";

// Replaces placeholder with button section in your web page
String relayState(int numRelay){
  if(!RELAY_NO){
    if(digitalRead(relayGPIOs[numRelay-1])){
      return "checked";
    }
    else {
      return "";
    }
  }
  else {
    if(digitalRead(relayGPIOs[numRelay-1])){
      return "";
    }
    else {
      return "checked";
    }
  }
  return "";
}

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    for(int i=1; i<=NUM_RELAYS; i++){
      String relayStateValue = relayState(i);
      buttons+= "<h4>Relay #" + String(i) + " - GPIO " + relayGPIOs[i-1] + "</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String(i) + "\" "+ relayStateValue +"><span class=\"slider\"></span></label>";
    }
    return buttons;
  }
  return String();
}


void dump_byte_array(byte * buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

bool isActive(){
  int count = 0;
  if(!mfrc522.PICC_IsNewCardPresent()){
    for(int j = 0; j < 2; j++){
      Serial.println(j);
      for (byte i = 0; i < 4; i++){   //tagarray's columns
          if(mfrc522.uid.uidByte[i] != UIDS[j][i]);
          else {
            if (i == mfrc522.uid.size - 1)                // Test if we browesed the whole UID.
              {
                count++;
              }
              else
              {
                continue;                                           // We still didn't reach the last cell/column : continue testing!
              }
          }
      }  
    }
    if(count <= 0) return false;
    else  return true;    
  }
  return false;
    
}

bool activated = false;

void active(){
  if(skill > 0){
    Serial.println("active");
    activated = true;
    //digitalWrite(SKILL_RELAY, HIGH);
    skill--;
    timing = timer; 
  }
}

void deactive(){
  Serial.println("active");
  activated = false; 
  digitalWrite(SKILL_RELAY, LOW);
  timing = timer;
  events.send(String(timing).c_str(),"timer",millis());
}

void setup() {

  Serial.begin(9600);           // Initialize serial communications with the PC
  while (!Serial);              // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

    SPI.begin();                  // Init SPI bus
    mfrc522.PCD_Init(SELECT, RST_PIN);   // Init MFRC522
    Serial.print(F("Reader "));
    Serial.print(F(": "));
    delay(4);
    mfrc522.PCD_DumpVersionToSerial();

    for(int i=1; i<=NUM_RELAYS; i++){
    pinMode(relayGPIOs[i-1], OUTPUT);
    if(RELAY_NO){
      digitalWrite(relayGPIOs[i-1], HIGH);
    }
    else{
      digitalWrite(relayGPIOs[i-1], LOW);
    }
  }

     initWiFi();

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
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
      Serial.println(inputMessage);
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
      Serial.println(inputMessage);
      if(RELAY_NO){
        Serial.print("NO ");
        digitalWrite(relayGPIOs[inputMessage.toInt()-1], !inputMessage2.toInt());
      }
      else{
        Serial.print("NC ");
        digitalWrite(relayGPIOs[inputMessage.toInt()-1], inputMessage2.toInt());
      }
    }
    else if (request->hasParam(PARAM_INPUT_3) & request->hasParam(PARAM_INPUT_4)){
      Serial.println("REQ2");
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;
      inputMessage2 = request->getParam(PARAM_INPUT_4)->value();
      inputParam2 = PARAM_INPUT_4;
      skill = inputMessage.toInt();
      timer = inputMessage2.toInt();
      timing = timer;
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
    
    // Looking for new cards
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() && (!activated)) {
      Serial.print(F("Reader "));
      Serial.println();

      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(": Card UID:"));
      mfrc522.PCD_StopCrypto1();
      dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
      if(isActive()) active();
    } 

    if (((millis() - lastTime2) > timerDelay2) && activated){
      digitalWrite(SKILL_RELAY, HIGH);
      delay(50);
      digitalWrite(SKILL_RELAY, LOW);
      delay(50);
      digitalWrite(SKILL_RELAY, HIGH);
      delay(50);
      digitalWrite(SKILL_RELAY, LOW);
      delay(100);
      digitalWrite(SKILL_RELAY, HIGH);
      delay(100);
      digitalWrite(SKILL_RELAY, LOW);
      lastTime2 = millis();
    }

    if (((millis() - lastTime) > timerDelay) && activated) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(String(skill).c_str(),"skill",millis());
    events.send(String(--timing).c_str(),"timer",millis());
    //Serial.println(timing); 
    if(timing < 1) deactive();

    lastTime = millis();
    }
    
}
