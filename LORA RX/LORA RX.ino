  // Lora Ra-02 Bidirectional Communication With Esp8266

#include <LoRa.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define SS 15
#define RST D4
#define DIO0 D0


// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"



// Insert Firebase project API Key
#define API_KEY "AIzaSyCYmVqm7sKqqZphrNcynQSmnQZBq1IBv9A"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "chaudharykapil8279@gmail.com"
#define USER_PASSWORD "Kapil8279#"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://sarathidb-f0479-default-rtdb.europe-west1.firebasedatabase.app/"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;
String parentPath;

FirebaseJson json;
//const char* ssid = "Ashish";
//const char* password = "12123456";
//const char* ssid = "KAPIL";
//const char* password = "pooja8279";
const char* ssid = "Samsung";
const char* password = "kapil8279";



String data = "";
bool stringComplete = false;      // Whether the string is complete
long long lasttime = 0;
int led = D2; 
int buzzer = D1;
WiFiClient client;
HTTPClient http;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
int timestamp;
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
  timeClient.begin();

  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config,&auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/crash-loc";


  //pinMode(buzzer,OUTPUT);
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
      //sendDataToDB(coord);
      sendToApi(coord);
      digitalWrite(led, 1);
      //tone(buzzer, 100, 5000);
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








void sendDataToDB(String coord){
  timestamp = getTime();
  Serial.print ("time: ");
  Serial.println (timestamp);
  Serial.print("Coord: ");
  
  String r1 = "",r2 = "";
  bool f=false;
  for(int x=0;x<coord.length();x++){
    if(coord[x] == ' '){
      f = true;
    }
    if(f){
      r2+=coord[x];
    }
    else{
      r1+=coord[x];
    }
  }

  parentPath= databasePath + "/" + String(timestamp);
  Serial.print(r1);
  Serial.println(r2);
  json.set("Lat", r1);
  json.set("Long", r2);
  json.set("timecode", String(timestamp));
  Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  
}

unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}
