#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define STEP 2

#define BCD3 22
#define BCD2 17
#define BCD1 16
#define BCD0 4

#define LE3 26 //33
#define LE2 13 //25
#define LE1 33 //26
#define LE0 25 //13

#define GREEN_RELAY 32
#define LUNCH_BUTTON 23
#define MOVE_BUTTON 5

#define ROT1_CLK 21
#define ROT1_DT  19
#define ROT1_SW  18

#define ROT1_GET_CLK()     digitalRead(ROT1_CLK)
#define ROT1_GET_DT()      digitalRead(ROT1_DT)
#define ROT1_GET_SW()      digitalRead(ROT1_SW)
#define ROT1_isSWPress()   ROT1_GET_SW() == LOW
#define ROT1_hasChanged()  ROT1_GET_CLK() != ROT1_GET_DT()
#define ROT1_changed()     ROT1_GET_CLK() == ROT1_GET_DT()
#define ROT1_isInc()       (ROT1_prevCLK != ROT1_GET_CLK())
#define ROT1_isDec()       !ROT1_isInc()

#define ROT2_CLK 27
#define ROT2_DT  14
#define ROT2_SW  12

#define ROT2_GET_CLK()     digitalRead(ROT2_CLK)
#define ROT2_GET_DT()      digitalRead(ROT2_DT)
#define ROT2_GET_SW()      digitalRead(ROT2_SW)
#define ROT2_isSWPress()   ROT2_GET_SW() == LOW
#define ROT2_hasChanged()  ROT2_GET_CLK() != ROT2_GET_DT()
#define ROT2_changed()     ROT2_GET_CLK() == ROT2_GET_DT()
#define ROT2_isInc()       (ROT2_prevCLK != ROT2_GET_CLK())
#define ROT2_isDec()       !ROT2_isInc()

#define NUMBER1_MAX       99
#define NUMBER1_MIN       0

#define NUMBER2_MAX       99
#define NUMBER2_MIN       0

#define bounce        0.01
int lastBounce = 0;

bool ROT1_changingFlag   = false;
bool ROT1_changedFlag    = false;
bool ROT1_swFlag         = false;

// Timer variables 
unsigned long lastTime = 0;  
unsigned long timerDelay = 1000;

unsigned long lastTime2 = 0;  
unsigned long timerDelay2 = 500;

unsigned long lastTime3 = 0;  
unsigned long timerDelay3 = 50;

unsigned long lastTime4 = 0;  
unsigned long timerDelay4 = 1000;

// Replace with your network credentials
const char* ssid = "dotsaout";
const char* password = "v123456789st";

const char* PARAM_INPUT_1 = "year";

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 23);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 0);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

uint8_t ROT1_prevCLK;
uint8_t ROT1_prevDT;

bool ROT2_changingFlag   = false;
bool ROT2_changedFlag    = false;
bool ROT2_swFlag         = false;

uint8_t ROT2_prevCLK;
uint8_t ROT2_prevDT;

#define maxMainYears  4
#define maxBonusYears 10

int mainYears[maxMainYears] = {1700, 1920, 2030, 2070};

bool checkNumInArr(int number, int arr[], int len){
  for(int i = 0; i < len; i++){
    if(number == arr[i]) return true;
  }
  return false;  
}

int prevMode = 1;
int mode = 1;
int lunch = 0;
int prevMove = 1;
int moveBtn = 1;
int move = 0;

int prevNumber = 0;
int number  = 0;
int number1 = 0;
int number2 = 0;

int _step1 = 0;
int _step2 = 0;

bool checkMainYear(){
  checkNumInArr(number, mainYears, maxMainYears);
}

void confirm(){
  if(checkMainYear()) {
    digitalWrite(GREEN_RELAY, HIGH);
    events.send(String(1).c_str(),"mode",millis());
  }
  else {
    digitalWrite(GREEN_RELAY, LOW);
    events.send(String(1).c_str(),"mode",millis());
  }
  
}



void number_1_incr(){
  if(_step1 == STEP){
    if(number1 < NUMBER1_MAX) number1++;
    else number1 = 0;
    _step1 = 0;
  }
  else _step1++;
}

void number_1_decr(){
  if(_step1 == STEP){
    if(number1 > NUMBER1_MIN) number1--; 
    else number1 = 99;
    _step1 = 0;
  }
  else _step1++;
}

void number_2_incr(){
  if(_step2 == STEP){
    if(number2 < NUMBER2_MAX) number2++;  
    else number2 = 0;
    _step2 = 0;
  }
  else _step2++;
}

void number_2_decr(){
  if(_step2 == STEP){
    if(number2 > NUMBER2_MIN) number2--;
    else number2 = 99;
    _step2 = 0;
  }
  else _step2++;   
}

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
  <title>YEAR COUNTER WEB SERVER</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;height: 100%; background-color: black;}
    .topnav { overflow: hidden; background-color: #000; color: #E11616; font-size: 1rem; }
    .content { padding: 20px; border: 1px solid #E11616;} //#2cc12e GREEN  #2616E1 blue
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 900px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
    .num { font-size: 60px; letter-spacing: 60px; color: #E11616;}
  </style>
</head>
<body>
  <div class="topnav">
    <h1>7 SEGMENT</h1>
  </div>
  <div class="content">
    <div class="cards">
        <h1 id="num" class="num"></h1>
      </div>
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

source.addEventListener('num', function(e) {
  console.log("num", e.data);
  document.getElementById("num").innerHTML = e.data;
 }, false);

 for (var i = 0; i < 4; i++) {
   source.addEventListener('segment' + i, function(e) {
    console.log("segment " + i + ": ", e.data);
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
    int j = 0;
    if(num < 10) j = 1;
    else if(num <100) j = 2;
    else if(num < 1000) j = 3;
    else j = 4; 
    for(int i = 0; i < 4; i++){
      segment[i] = 0;
    }
    for(int i = 0; i < j; i++){
      segment[i] = number % 10;
      number /= 10;  
    }
  }
  else {
     
  }
}

String processor(const String& var){
  return String();
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

void setup(){
  Serial.begin(9600);

  for(int i = 0; i < 4; i++){
    pinMode(BCDS[i], OUTPUT);
    digitalWrite(BCDS[i], LOW);
  }

  for(int i = 0; i < 4; i++){
    pinMode(LATCH[i], OUTPUT);
    digitalWrite(LATCH[i], LOW);
  }

  pinMode(GREEN_RELAY, OUTPUT);
  pinMode(LUNCH_BUTTON, INPUT_PULLUP);
  pinMode(MOVE_BUTTON, INPUT_PULLUP);
  digitalWrite(GREEN_RELAY, LOW);

  pinMode(ROT1_SW, INPUT_PULLUP);

  ROT1_prevCLK = ROT1_GET_CLK();
  ROT1_prevDT  = ROT1_GET_DT();

  pinMode(ROT2_SW, INPUT_PULLUP);

  ROT2_prevCLK = ROT2_GET_CLK();
  ROT2_prevDT  = ROT2_GET_DT();
  
  initWiFi();

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/update?relay=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      number = inputMessage.toInt();
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

void loop(){
  number = number2 * 100 + number1;
  if(millis() - lastBounce > bounce){
    if(ROT1_hasChanged() && !ROT1_changingFlag) {
    if(ROT1_isInc())       number_1_incr();
    else if(ROT1_isDec())  number_1_decr();
    else              Serial.println("ERROR");
    ROT1_changingFlag = true;
    ROT1_changedFlag  = false;
    }

    if(ROT1_changed() && !ROT1_changedFlag) {
      ROT1_prevCLK = ROT1_GET_CLK();
      ROT1_prevDT  = ROT1_GET_DT();
      ROT1_changingFlag = false;
      ROT1_changedFlag  = true;
    } 

    if(ROT2_hasChanged() && !ROT2_changingFlag) {
    if(ROT2_isInc())       number_2_incr();
    else if(ROT2_isDec())  number_2_decr();
    else              Serial.println("ERROR");
    ROT2_changingFlag = true;
    ROT2_changedFlag  = false;
    }

    if(ROT2_changed() && !ROT2_changedFlag) {
      ROT2_prevCLK = ROT2_GET_CLK();
      ROT2_prevDT  = ROT2_GET_DT();
      ROT2_changingFlag = false;
      ROT2_changedFlag  = true;
    } 

    lastBounce = millis();
      
  }
  
  //if(checkMainYear() || checkBonusYear()) SerialPort1.print(-2);
  mode = digitalRead(LUNCH_BUTTON);
  moveBtn = digitalRead(MOVE_BUTTON);
  Serial.println(moveBtn);
  if (prevMove != moveBtn && (millis() - lastTime3 > timerDelay3)){
    if(moveBtn == 0) {
    events.send(String(1).c_str(),"move",millis());
    }
    else {
      events.send(String(0).c_str(),"move",millis());
    }        
    prevMove = moveBtn;
    lastTime3 = millis();
  }
  if (prevMode != mode && (millis() - lastTime > timerDelay)){
    if(mode == 0) {
    events.send(String(1).c_str(),"mode",millis());
    lunch = 1;
    }
    else {
      events.send(String(0).c_str(),"mode",millis());
    }
    prevMode = mode;
    lastTime = millis();
  }
  
  if (number == 1488 || number == 529 || number == 2072|| number == 2090) {
    if (lunch == 1){
      digitalWrite(GREEN_RELAY, HIGH);
    }
    else if(millis() - lastTime2 > timerDelay2){
      digitalWrite(GREEN_RELAY, HIGH);
      delay(25);
      digitalWrite(GREEN_RELAY, LOW);
        
      lastTime2 = millis();
    }
    
  }
  else {
    digitalWrite(GREEN_RELAY, LOW);
    lunch = 0;
  }
    
  if(number != prevNumber && (millis() - lastTime4 > timerDelay4)){
    setSegmentNumber(number);
    events.send(String(number).c_str(),"num",millis());
    lunch = 0;
    prevNumber = number;
    showNumber();
    lastTime4 = millis();
  }
  
}
