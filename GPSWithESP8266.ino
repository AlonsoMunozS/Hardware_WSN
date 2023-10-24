#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

TinyGPSPlus gps;
SoftwareSerial SerialGPS(4, 5); 

const char* ssid = "WIFI"; //ssid de tu wifi
const char* password = "12345678901"; //contraseña de tu wifi
const char* mqtt_server = "192.168.203.22"; // Reemplaza esto con la dirección IP de tu broker Mosquitto
// const char* ssid = "PanConQueso"; //ssid of your wifi
// const char* password = "P4nS1nQu3s0"; //password of your wifi#include <TinyGPS++.h>
// const char* mqtt_server = "192.168.2.107"; // Replace with your Mosquitto broker IP address
const int led = 13;
const int button = 16;
int temp = 0;

float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString , LatitudeString , LongitudeString;
WiFiClient espClient;
PubSubClient client(espClient);
void setup()
{
  Serial.begin(9600);
  SerialGPS.begin(9600);
  Serial.println();
  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883); // Port 1883 is the default MQTT port
    pinMode(led, OUTPUT);
  pinMode(button, INPUT);

}

void loop()
{
  while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read()))
    {
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


  
  if (gps.location.isValid())
  {
    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["latitude"] = Latitude;
    jsonDoc["longitude"] = Longitude;
    jsonDoc["fecha"] = DateString;
    jsonDoc["hora"] = TimeString;
    

    String jsonData;
    serializeJson(jsonDoc, jsonData);
    Serial.println(jsonData);
      if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publish a message to the topic "esp8266/sensor"
  client.publish("GPS", jsonData.c_str());
  delay(5000);

}
else {
  Serial.println("Señal GPS no disponible");
  delay(1000);
}
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}