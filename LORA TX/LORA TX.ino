#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <LoRa.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define SS 15
#define RST D4
#define DIO0 D0

TinyGPSPlus gps;
SoftwareSerial SerialGPS(D3, 10); 
Adafruit_MPU6050 accel;

String data = "";
bool stringComplete = false;

//const char* ssid = "Tenda_3A5220";
//const char* password = "DILLELEMERA";
//const char* ssid = "Tenda_410840";
//const char* password = "Dillelemera";
const char* ssid = "Samsung";
const char* password = "kapil8279";

//const char* ssid = "KAPIL";
//const char* password = "pooja8279";

float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString , LatitudeString , LongitudeString;
float oldx = 0,oldy = 0,oldz = 0;

WiFiServer server(80);

void INITMPU(){
  accel.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (accel.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }
  accel.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (accel.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
  }

  accel.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (accel.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }

  Serial.println("Accelerometer Setup");
  delay(100);
}
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
  INITMPU();
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
  //SendDataTest();
  
  SendData();
  while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read())){
      if(gps.satellites.value()){    
        Serial.println(gps.satellites.value());
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
  sensors_event_t event, g, temp;
  accel.getEvent(&event, &g, &temp);
  float x = event.acceleration.x;
  float y = event.acceleration.y;
  float z = event.acceleration.z;
  //Serial.print("X: "); Serial.print(x); Serial.print("\t");
  //Serial.print("Y: "); Serial.print(y); Serial.print("\t");
  //Serial.print("Z: "); Serial.print(z); Serial.print("\n");
  float mag = sqrt(sq(x) + sq(y) + sq(z));
  
  //oldx = x;
  //oldy = y;
  //oldz = z;
  //Serial.print("Mag: ");
  Serial.println(mag);
  if(mag > 12){
    return true;
  }
  else{
    return false;
  }
  //Serial.println("m/s^2 ");
}


void TestFunc() {

  if(true){
  //if(checkAcceident()){
    Serial.print("Sending Message: ");
    //Serial.println(LatitudeString + " " + LongitudeString);

    LoRa.beginPacket();
    LoRa.print("28.45084 77.29558");
    //LoRa.print(LatitudeString + " " + LongitudeString);
    LoRa.endPacket();
    delay(200);
    
    
  }
}

void SendDataTest() {

  if(true && checkAcceident()){
  //if(checkAcceident()){
    double lat = 23.207401;
    double longi = 72.585048;
    Serial.print("Sending Message: ");
    
    lat += (double)random(-1,1)/100;
    longi += (double)random(-1,1)/100;
    char a1[12];
    char a2[12];
    dtostrf(lat,6,4,a1);
    dtostrf(longi,6,4,a2);
    
    Serial.println(String(a1) + " " + String(a2));
    LoRa.beginPacket();
    LoRa.print(String(a1) + " " + String(a2));
    //LoRa.print(LatitudeString + " " + LongitudeString);
    LoRa.endPacket();
    delay(5000);
    
    
  }
  delay(500);
}






