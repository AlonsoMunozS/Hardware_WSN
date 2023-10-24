import paho.mqtt.client as mqtt
import json

# Función de callback cuando el cliente se conecta al broker MQTT
def on_connect(client, userdata, flags, rc):
    print("Conectado con el código de resultado: " + str(rc))
    # Suscribirse a un tema (topic) en el broker
    client.subscribe("GPS")

# Función de callback cuando se recibe un mensaje en el tema suscrito
def on_message(client, userdata, msg):
    json_object = json.loads(msg.payload.decode())
    # Procesar el mensaje recibido aquí
    print(json_object)

# Configuración del cliente MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Dirección del broker MQTT y puerto
broker_address = "192.168.2.107"
port = 1883

# Conectar al broker MQTT
client.connect(broker_address, port, 60)

# Mantener la conexión MQTT activa
client.loop_forever()
