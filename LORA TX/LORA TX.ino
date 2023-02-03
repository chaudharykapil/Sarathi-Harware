  // Lora Ra-02 Bidirectional Communication With Esp8266

#include <LoRa.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include<ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SS 15
#define RST 16
#define DIO0 4

const char* ssid = "Tenda_3A5220";
const char* password = "DILLELEMERA";


String data = "";
bool stringComplete = false;      // Whether the string is complete
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
}


 
void loop()
{
  //SendData();
  RecieveData();
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
    }
  }
}
void sendToApi(String latlong){
  if(WiFi.status() == WL_CONNECTED){
    WiFiClient cl;
    HTTPClient http;
    String serverPath = serverName + "/?latlong=" + latlong;
    http.begin(cl,serverPath);
    int code = http.GET();
    if(code > 0){
      String payload = http.getString();
      Serial.println(payload);
      delay(2000);
    }
    else{
      Serial.print("Error code");
      Serial.println(code);
    }
  }
}
















