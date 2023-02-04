  // Lora Ra-02 Bidirectional Communication With Esp8266

#include <LoRa.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include<ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SS 15
#define RST D4
#define DIO0 D3

//const char* ssid = "Tenda_3A5220";
//const char* password = "DILLELEMERA";
//const char* ssid = "Tenda_410840";
//const char* password = "Dillelemera";
const char* ssid = "Ashish";
const char* password = "12123456";

//const char* ssid = "KAPIL";
//const char* password = "pooja8279";


String data = "";
bool stringComplete = false;      // Whether the string is complete
long long lasttime = 0;
int led = D2; 
int buzzer = D1;
WiFiClient client;
HTTPClient http;
String serverName = "http://kapil829.pythonanywhere.com";
void setup()
{
  Serial.begin(9600);
  while (!Serial);  
  
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Error");
    delay(100);
    while (1);
  }
  else{
    Serial.println("LORA Device Activated");
  }
  data.reserve(200);        // Max Number of character Sended through a Single mssage
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    yield();
  }
  Serial.println("Connected to the WiFi network");
  pinMode(buzzer,OUTPUT);
  pinMode(led,OUTPUT);
}


 
void loop()
{
  //SendData();
  RecieveData();
  if(millis()-lasttime > 5000){
    digitalWrite(led, 0);
  }
  delay(200);
  
}
void RecieveData(){
  stringComplete = false;
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      Serial.print("Message Received : ");
      String coord = LoRa.readString();
      Serial.println(coord);
      sendToApi(coord);
      digitalWrite(led, 1);
      tone(buzzer, 100, 5000);
      lasttime = millis();

    }
  }
}
void sendToApi(String latlong){
  if(WiFi.status() == WL_CONNECTED){
    WiFiClient cl;
    HTTPClient http;
    String serverPath = serverName + "/api/sendcoord?latlong=" + latlong;
    http.begin(cl,serverPath);
    Serial.println(serverPath);
    int code = http.GET();
    if(code > 0){
      String payload = http.getString();
      Serial.println(payload);
      delay(200);
    }
    else{
      Serial.print("Error code");
      Serial.println(code);
    }
  }
}
