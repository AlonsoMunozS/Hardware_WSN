#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

TinyGPSPlus gps;
SoftwareSerial SerialGPS(4, 5); // Inicializa comunicación serial para el módulo GPS en pines 4 y 5

const char *ssid = "NOMBRE_DE_TU_RED_WIFI";               // Reemplaza con el nombre de tu red WiFi
const char *password = "CONTRASEÑA_DE_TU_RED_WIFI";       // Reemplaza con la contraseña de tu red WiFi
const char *mqtt_server = "DIRECCION_IP_DEL_BROKER_MQTT"; // Reemplaza con la dirección IP de tu broker MQTT
const int led = 13;                                       // Pin para el indicador LED
const int button = 16;                                    // Pin para la entrada del botón
int temp = 0;

float Latitude, Longitude;
int year, month, date, hour, minute, second;
String DateString, TimeString, LatitudeString, LongitudeString;
WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(9600);    // Inicializa la comunicación serial
  SerialGPS.begin(9600); // Inicializa la comunicación serial con el módulo GPS
  Serial.println();
  Serial.print("Conectando");
  WiFi.begin(ssid, password); // Conéctate a la red WiFi

  // Espera hasta que se establezca la conexión a WiFi
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println(WiFi.localIP());

  // Conéctate al broker MQTT
  client.setServer(mqtt_server, 1883); // Configura el servidor y puerto del broker MQTT
  pinMode(led, OUTPUT);                // Configura el pin del LED como salida
  pinMode(button, INPUT);              // Configura el pin del botón como entrada
}

void loop()
{
  // Lee los datos del GPS
  while (SerialGPS.available() > 0)
  {
    if (gps.encode(SerialGPS.read()))
    {
      if (gps.location.isValid())
      {
        Latitude = gps.location.lat();
        LatitudeString = String(Latitude, 6);
        Longitude = gps.location.lng();
        LongitudeString = String(Longitude, 6);
      }
      if (gps.date.isValid())
      {
        DateString = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        // Formatea la fecha
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
        hour = gps.time.hour() + 5; // Ajusta a UTC
        minute = gps.time.minute();
        second = gps.time.second();

        // Formatea la hora
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
  }

  // Si los datos del GPS son válidos, crea un objeto JSON y publícalo en el tema MQTT
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

    // Verifica la conexión MQTT y publica los datos en el tema "GPS"
    if (!client.connected())
    {
      reconnect();
    }
    client.loop();

    // Publica un mensaje en el tema "GPS"
    client.publish("GPS", jsonData.c_str());
    delay(5000); // Espera 5 segundos antes de enviar los próximos datos
  }
  else
  {
    Serial.println("Señal GPS no disponible"); // Imprime un mensaje si la señal del GPS no está disponible
    delay(1000);
  }
}

// Reintentar la conexión al broker MQTT si se pierde la conexión
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Intentando conexión MQTT...");
    if (client.connect("ESP8266Client"))
    {
      Serial.println("conectado");
    }
    else
    {
      Serial.print("falló, estado=");
      Serial.print(client.state());
      Serial.println(" intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}
