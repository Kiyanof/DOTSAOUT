#include <LedControl.h>
#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#endif

#define DIN_PIN   D7
#define CS_PIN    D8
#define CLK_PIN   D5

LedControl disp = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 3);

#define NORMALL_EYE  0X00183c7e7e3c1800

// HALLOWIN EYE
#define HALLOWIN_LEFT_EYE   0x387ce4e6fe1f0301
#define HALLOWIN_RIGHT_EYE  0x1c3e39797ff8e080
//

//VERY ANGRY EYE
#define VANGRY_LEFT_EYE   0x3c42814d2d110907
#define VANGRY_RIGHT_EYE  0x3c4281b2b48890e0
//

// HAPPY EYE
#define HAPPY_LEFT_EYE   0x7e81a59981828478
#define HAPPY_RIGHT_EYE  0x7e81a5998141211e
//
// SLEEPY EYE
#define SLEEPY_LEFT_EYE   0x7e81f991a1fa8478
#define SLEEPY_RIGHT_EYE  0x7e81b991a179211e
//
// DEPPRESS EYE
#define DEPRESS_LEFT_EYE   0x7e81b1b181828478
#define DEPRESS_RIGHT_EYE  0x7e818d8d8141211e
//
// DEPPRESS EYE
#define BLINK_LEFT_EYE   0x7e81ffff81828478
#define BLINK_RIGHT_EYE  0x7e81ffff8141211e
//

#define SMILE_MOUTH  0X187e668181000000
#define SAD_MOUTH    0x81e77e1800000000
#define POKER_MOUTH  0x00ffff0000000000
#define GIGI_MOUTH   0xaaaa555500000000

const uint64_t RIGHT_EYE_MOVE[] = {
  0x7e8d938d8141211e,
  0x7e99a5998141211e,
  0x7eb1c9b18141211e
};

const uint64_t LEFT_EYE_MOVE[] = {
  0x7e8d938d8141211e,
  0x7e99a5998141211e,
  0x7eb1c9b18141211e
};

const uint64_t RIGHT_EYE_BLINK[] = {
  0x7e99a5998141211e,
  0x7e99a59941211e00,
  0x7e99a559211e0000,
  0x7e8955293e000000,
  0x7e8d4d3e00000000
};

const uint64_t LEFT_EYE_BLINK[] = {
  0x7e8d938d8141211e,
  0x7e8d938d41211e00,
  0x7e8d934d211e0000,
  0x7e8d532d1e000000,
  0x7e4d2d1e00000000
};

void displayImage(uint64_t left, uint64_t right, uint64_t mouth) {
  for (int i = 0; i < 8; i++) {
    byte row = (left >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      disp.setLed(0, i, j, bitRead(row, j));
    }
  }

  for (int i = 0; i < 8; i++) {
    byte row = (right >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      disp.setLed(1, i, j, bitRead(row, j));
    }
  }

  for (int i = 0; i < 8; i++) {
    byte row = (mouth >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      disp.setLed(2, i, j, bitRead(row, j));
    }
  }
}

void normall(){
  for(int i = 0;i< 3;i++){
    displayImage(LEFT_EYE_MOVE[i], RIGHT_EYE_MOVE[i], SAD_MOUTH);
    delay(random(50,100));  
  }
  for(int i = 0;i< 5;i++){
    displayImage(LEFT_EYE_BLINK[i], RIGHT_EYE_BLINK[i], SAD_MOUTH);
    delay(25);  
  }  
  for(int i = 1;i< 5;i++){
    displayImage(LEFT_EYE_BLINK[5 - i], RIGHT_EYE_BLINK[5 - i], SAD_MOUTH);
    delay(25);  
  } 
}

// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 1000;

const char* ssid = "dotsaout";
const char* password = "v123456789st";

const char* PARAM_INPUT_1 = "face";  

IPAddress local_IP(192,168,1,25);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255, 255, 255, 0);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

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
  <h2>ROBOT</h2>
  

<script>function conf(state) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/update?face="+sate, true);
  xhr.send();
}</script>
</body>
</html>

  

)rawliteral";

// Replaces placeholder with button section in your web page

// Replaces placeholder with button section in your web page
String processor(const String& var){
  return String();
}

void setup() {
  Serial.begin(9600);
  
   // Connect to Wi-Fi
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
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
      int state = inputMessage.toInt();    
      switch (state){
        case 0: displayImage(VANGRY_LEFT_EYE, VANGRY_RIGHT_EYE, POKER_MOUTH);  break;
        case 1: displayImage(HAPPY_LEFT_EYE, HAPPY_RIGHT_EYE, SMILE_MOUTH);  break;
        case 2: displayImage(BLINK_LEFT_EYE, HAPPY_RIGHT_EYE, SMILE_MOUTH); delay(50);
                displayImage(HAPPY_LEFT_EYE, BLINK_RIGHT_EYE, SMILE_MOUTH); break;
        case 3: displayImage(DEPRESS_LEFT_EYE, DEPRESS_RIGHT_EYE, POKER_MOUTH);  break;
        case 4: displayImage(DEPRESS_LEFT_EYE, DEPRESS_RIGHT_EYE, SAD_MOUTH);  break; 
        case 5: displayImage(SLEEPY_LEFT_EYE, SLEEPY_RIGHT_EYE, GIGI_MOUTH);   break;    
        case 6: displayImage(HALLOWIN_LEFT_EYE, HALLOWIN_RIGHT_EYE, SMILE_MOUTH); break;
        case 7: normall(); break;
        default: normall();                        
      }
      
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });
  // Start server
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.begin();

  for(int index=0;index<disp.getDeviceCount();index++) {
        disp.clearDisplay(index);
        disp.setIntensity(index, 10); 
        disp.shutdown(index,false); 
    } 
  
}

void loop() {

}
