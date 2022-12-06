#include <ESP8266WiFi.h>
#include <string>

#define LAMP1       D0
#define LAMP2       D1
#define LAMP3       D2
#define LAMP4       D3
#define LAMP5       D4
#define LAMP6       D5
#define LAMP7       D6
#define LAMP8       D7

const uint8_t number  = 8;
uint8_t LAMPS[number] = {LAMP1, LAMP2, LAMP3, LAMP4, LAMP5, LAMP6, LAMP7, LAMP8};
int lamps[number];
String ids[number] =  {"1", "2", "3", "4", "5", "6", "7", "8"};

const char* ssid = "dotsaout";
const char* password = "v123456789st";

WiFiServer server(80);


IPAddress local_IP(192, 168, 1, 37);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);


void setup() {

  for(int i = 0; i < number; i++){
    pinMode(LAMPS[i], OUTPUT);
    digitalWrite(LAMPS[i], LOW);
    lamps[i] = LOW;
  }

  Serial.begin(9600);
  Serial.println();
  Serial.print("Wifi connecting to ");
  Serial.println( ssid );

  WiFi.hostname("MASHAL RELAY ");
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
    }
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecting");

  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }


  Serial.println();

  Serial.println("Wifi Connected Success!");
  Serial.print("NodeMCU IP Address : ");
  Serial.println(WiFi.localIP() );

  server.begin();
  Serial.println("NodeMCU Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}

void loop() {

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("Hello New client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  for(int i = 0; i < number; i++){
    if (request.indexOf("/lamp" + ids[i] + "=OFF") != -1)  {
      digitalWrite(LAMPS[i], LOW);
      lamps[i] = LOW;
    } 
    if (request.indexOf("/lamp" + ids[i] + "=ON") != -1)  {
      digitalWrite(LAMPS[i], HIGH);
      lamps[i] = HIGH;
    } 
    digitalWrite(LAMPS[i], lamps[i]);
  }

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  for(int i = 0; i < number; i++){
    client.print("LAMP " + ids[i] + ": ");

    if (lamps[i] == LOW) {
      client.print("OFF");
    } else {
      client.print("ON");
    }
    client.println("<br><br>");
    client.println("<a href=\"/lamp" + ids[i] + "=OFF\"\"><button>OFF </button></a>");
    client.println("<a href=\"/lamp" + ids[i] +"=ON\"\"><button>ON </button></a><br />");   
  }

  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

}
