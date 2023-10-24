#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// const char* ssid = "WIFI"; //ssid de tu wifi
// const char* password = "12345678901"; //contraseña de tu wifi
// const char* mqtt_server = "192.168.203.22"; // Reemplaza esto con la dirección IP de tu broker Mosquitto
const char* ssid = "PanConQueso"; //ssid de tu wifi
const char* password = "P4nS1nQu3s0"; //contraseña de tu wifi
const char* mqtt_server = "192.168.2.107";
const int led = 13;
const int led2 = 12;

const int button = 16;
int temp = 0;
int tempAnt = temp;


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.print("Conectando");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");

  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883); // El puerto 1883 es el puerto MQTT predeterminado
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(button, INPUT);
  digitalWrite(led2, HIGH);

}

void loop() {
  tempAnt= temp;
  temp = digitalRead(button);
  if (!client.connected()) {
    reconnect();
  }

  if (temp == HIGH && tempAnt != temp) {
    
    // Crear un objeto JSON
    StaticJsonDocument<200> jsonDocument;
    jsonDocument["used"] = true;

    // Convertir el objeto JSON a una cadena
    char buffer[200];
    serializeJson(jsonDocument, buffer);

    // Publicar el JSON en el tema "Button"
    client.publish("Button", buffer);
    digitalWrite(led, HIGH);
    digitalWrite(led2, LOW);

    delay(100);
  } else if(temp == LOW && tempAnt != temp) {
    // Crear un objeto JSON
    StaticJsonDocument<200> jsonDocument;
    jsonDocument["used"] = false;

    // Convertir el objeto JSON a una cadena
    char buffer[200];
    serializeJson(jsonDocument, buffer);

    // Publicar el JSON en el tema "Button"
    client.publish("Button", buffer);
    digitalWrite(led2, HIGH);
    digitalWrite(led, LOW);

    delay(100);
  }
}

void reconnect() {
  // Bucle hasta que estemos reconectados
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    // Intentar conectar
    if (client.connect("ESP8266Client")) {
      Serial.println("conectado");
    } else {
      Serial.print("falló, rc=");
      Serial.print(client.state());
      Serial.println(" intentando de nuevo en 5 segundos");
      // Esperar 5 segundos antes de intentar de nuevo
      delay(5000);
    }
  }
}
