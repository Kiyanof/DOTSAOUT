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

#define RST_PIN    D3  //
#define SS_PIN    D1    //
#define SKILL_RELAY   9
#define DAMAGE_RELAY  10

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC[4];
const int idPICC_length = 2;
byte valid_idPICC[idPICC_length][4] = {{0x23, 0xF3, 0x0A, 0x19},
                    {0x2c, 0x0F, 0x0B, 0x49}};

byte corom_idPICC[idPICC_length][4] = {{0x4A, 0xD0, 0x72, 0xBE},
                                      {0x9C, 0xF8, 0x17, 0x4A}};

bool read();
bool isActive();
bool active = false;

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
IPAddress local_IP(192, 168, 1, 38);
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
int corom = 0;
int prevCorom = 0;

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
  <h2>STOP WATCH</h2>
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

void setup() {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  delay(50);
  Serial.print(F("Reader "));
  Serial.print(F(": "));
  rfid.PCD_DumpVersionToSerial();
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

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
  if(read()){
    if(isActive()) {
      Serial.println(F("ACTIVE"));
      events.send(String(1).c_str(),"active",millis());
      active = true;
      skill--;
    }
    else if(isCorom()) {
      Serial.println(F("COROM"));
      events.send(String(1).c_str(),"corom",millis());
    }
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  if (((millis() - lastTime2) > timerDelay2) && active){

      digitalWrite(SKILL_RELAY, HIGH);
      delay(100);
      digitalWrite(SKILL_RELAY, LOW);
      lastTime2 = millis();

    }

    if (((millis() - lastTime) > timerDelay) && active) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(String(skill).c_str(),"skill",millis());
    events.send(String(--timing).c_str(),"timer",millis());
    //Serial.println(timing);
    if(timing < 1) {
      Serial.println("DEACTIVE");
      active = false;
      digitalWrite(SKILL_RELAY, LOW);
      timing = timer;
      events.send(String(timing).c_str(),"timer",millis());
      events.send(String(0).c_str(),"active",millis());
    }

    lastTime = millis();
    }



}


/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(' ');
    Serial.print(buffer[i], DEC);
  }
}

bool read(){
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return false;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return false;
  return true;
}

bool check(int i){
  return rfid.uid.uidByte[0] == valid_idPICC[i][0] ||
         rfid.uid.uidByte[1] == valid_idPICC[i][1] ||
         rfid.uid.uidByte[2] == valid_idPICC[i][2] ||
         rfid.uid.uidByte[3] == valid_idPICC[i][3];
}

bool checkCorom(int i){
  return rfid.uid.uidByte[0] == corom_idPICC[i][0] ||
         rfid.uid.uidByte[1] == corom_idPICC[i][1] ||
         rfid.uid.uidByte[2] == corom_idPICC[i][2] ||
         rfid.uid.uidByte[3] == corom_idPICC[i][3];
}

bool isActive(){
  for(int i = 0; i < idPICC_length; i++){
    if(check(i)){
      return true;
    }
  }
  return false;
}

bool isCorom(){
  for(int i = 0; i < idPICC_length; i++){
    if(checkCorom(i)){
      return true;
    }
  }
  return false;
}
