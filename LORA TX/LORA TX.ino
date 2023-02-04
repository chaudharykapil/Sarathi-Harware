#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <LoRa.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define SS 15
#define RST D0
#define DIO0 D4

TinyGPSPlus gps;
SoftwareSerial SerialGPS(D3, 10); 
Adafruit_MPU6050 mpu;

String data = "";
bool stringComplete = false;

//const char* ssid = "Tenda_3A5220";
//const char* password = "DILLELEMERA";
//const char* ssid = "Tenda_410840";
//const char* password = "Dillelemera";
const char* ssid = "Ashish";
const char* password = "12123456";

//const char* ssid = "KAPIL";
//const char* password = "pooja8279";

float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString , LatitudeString , LongitudeString;
float oldx = 0,oldy = 0,oldz = 0;

WiFiServer server(80);
void setup()
{
  Serial.begin(9600);
  SerialGPS.begin(9600);
  Serial.println();
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Error");
    delay(100);
  }
  else{
    Serial.println("LORA Device Activated");
  }
  
  data.reserve(200);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  if(!accel.begin())
   {
      Serial.println("No valid sensor found");
   }
   else{
     Serial.println("Accelerometer Connected");
   }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());
    
}

void loop()
{
  SendData();
  while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read())){
      if(gps.satellites.value()){    
        //Serial.println(gps.satellites.value());
      }
      if (gps.location.isValid())
      {
        Latitude = gps.location.lat();
        LatitudeString = String(Latitude , 6);
        Longitude = gps.location.lng();
        LongitudeString = String(Longitude , 6);
      }

      if (gps.date.isValid())
      {
        DateString = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
        DateString = '0';
        DateString += String(date);

        DateString += " / ";

        if (month < 10)
        DateString += '0';
        DateString += String(month);
        DateString += " / ";

        if (year < 10)
        DateString += '0';
        DateString += String(year);
      }

      if (gps.time.isValid())
      {
        TimeString = "";
        hour = gps.time.hour()+ 5; //adjust UTC
        minute = gps.time.minute();
        second = gps.time.second();
    
        if (hour < 10)
        TimeString = '0';
        TimeString += String(hour);
        TimeString += " : ";

        if (minute < 10)
        TimeString += '0';
        TimeString += String(minute);
        TimeString += " : ";

        if (second < 10)
        TimeString += '0';
        TimeString += String(second);
      }
      

    }
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  //Response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>NEO-6M GPS Readings</title> <style>";
  s += "table, th, td {border: 1px solid blue;} </style> </head> <body> <h1  style=";
  s += "font-size:300%;";
  s += " ALIGN=CENTER>NEO-6M GPS Readings</h1>";
  s += "<p ALIGN=CENTER style=""font-size:150%;""";
  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  s += "width:50%";
  s += "> <tr> <th>Latitude</th>";
  s += "<td ALIGN=CENTER >";
  s += LatitudeString;
  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  s += LongitudeString;
  s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  s += DateString;
  s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  s += TimeString;
  s += "</td>  </tr> </table> ";
 
  
  if (gps.location.isValid())
  {
    s += "<p align=center><a style=""color:RED;font-size:125%;"" href=""http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
    s += LatitudeString;
    s += "+";
    s += LongitudeString;
    s += """ target=""_top"">Click here</a> to open the location in Google Maps.</p>";
    
  }

  s += "</body> </html> \n";

  client.print(s);
  
  delay(100);

}

void RecieveData(){
  stringComplete = false;
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      Serial.print("Message Received : ");
      Serial.println(LoRa.readString());
    }
  }
}

void SendData() {

  if(gps.location.isValid() && checkAcceident()){
  //if(checkAcceident()){
    Serial.print("Sending Message: ");
    Serial.println(LatitudeString + " " + LongitudeString);

    LoRa.beginPacket();
    //LoRa.print("28.45084 77.29558");
    LoRa.print(LatitudeString + " " + LongitudeString);
    LoRa.endPacket();
    delay(200);
    
    
  }
}

bool checkAcceident(){
  sensors_event_t event; 
  accel.getEvent(&event);
  float x = event.acceleration.x;
  float y = event.acceleration.y;
  float z = event.acceleration.z;
  //Serial.print("X: "); Serial.print(x); Serial.print("");
  //Serial.print("Y: "); Serial.print(y); Serial.print("");
  //Serial.print("Z: "); Serial.print(z); Serial.print("");
  float mag = sqrt(sq(x) + sq(y) + sq(z));
  
  //oldx = x;
  //oldy = y;
  //oldz = z;
  //Serial.println(mag);
  if(mag > 20){
    return true;
  }
  else{
    return false;
  }
  //Serial.println("m/s^2 ");
}