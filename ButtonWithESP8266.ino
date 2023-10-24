#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "NombreDeTuRedWiFi";               // Reemplaza con el nombre de tu red WiFi
const char *password = "TuContraseñaWiFi";            // Reemplaza con la contraseña de tu red WiFi
const char *mqtt_server = "DirecciónIPDelBrokerMQTT"; // Reemplaza con la dirección IP de tu broker MQTT
const int led = 13;                                   // Pin para el primer LED
const int led2 = 12;                                  // Pin para el segundo LED
const int button = 16;                                // Pin para el botón
int temp = 0;
int tempAnt = temp;

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(9600); // Inicializa la comunicación serial
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
  client.setServer(mqtt_server, 1883); // Configura el servidor y puerto del broker MQTT
  pinMode(led, OUTPUT);                // Configura el primer LED como salida
  pinMode(led2, OUTPUT);               // Configura el segundo LED como salida
  pinMode(button, INPUT);              // Configura el pin del botón como entrada
  digitalWrite(led2, HIGH);            // Apaga el segundo LED al inicio
}

void loop()
{
  tempAnt = temp;
  temp = digitalRead(button); // Lee el estado del botón

  if (!client.connected())
  {
    reconnect();
  }

  // Si el botón es presionado
  if (temp == HIGH && tempAnt != temp)
  {
    // Crear un objeto JSON
    StaticJsonDocument<200> jsonDocument;
    jsonDocument["used"] = true;

    // Convertir el objeto JSON a una cadena
    char buffer[200];
    serializeJson(jsonDocument, buffer);

    // Publicar el JSON en el tema "Button"
    client.publish("Button", buffer);

    // Enciende el primer LED y apaga el segundo LED
    digitalWrite(led, HIGH);
    digitalWrite(led2, LOW);

    delay(100);
  }
  // Si el botón es liberado
  else if (temp == LOW && tempAnt != temp)
  {
    // Crear un objeto JSON
    StaticJsonDocument<200> jsonDocument;
    jsonDocument["used"] = false;

    // Convertir el objeto JSON a una cadena
    char buffer[200];
    serializeJson(jsonDocument, buffer);

    // Publicar el JSON en el tema "Button"
    client.publish("Button", buffer);

    // Apaga el primer LED y enciende el segundo LED
    digitalWrite(led2, HIGH);
    digitalWrite(led, LOW);

    delay(100);
  }
}

// Reintentar la conexión al broker MQTT si se pierde la conexión
void reconnect()
{
  // Bucle hasta que estemos reconectados
  while (!client.connected())
  {
    Serial.print("Intentando conexión MQTT...");
    // Intentar conectar
    if (client.connect("ESP8266Client"))
    {
      Serial.println("conectado");
    }
    else
    {
      Serial.print("falló, rc=");
      Serial.print(client.state());
      Serial.println(" intentando de nuevo en 5 segundos");
      // Esperar 5 segundos antes de intentar de nuevo
      delay(5000);
    }
  }
}
