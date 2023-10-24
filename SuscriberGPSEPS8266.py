import paho.mqtt.client as mqtt
from pymongo import MongoClient
import json

client = MongoClient('mongodb://Alonso:1234Alonso@ac-ouxjhz4-shard-00-00.q41p8o6.mongodb.net:27017,ac-ouxjhz4-shard-00-01.q41p8o6.mongodb.net:27017,ac-ouxjhz4-shard-00-02.q41p8o6.mongodb.net:27017/?replicaSet=atlas-8gtwdq-shard-0&authSource=admin&tls=true')
db = client['PruebaPMM']  # Reemplaza 'tu_base_de_datos' con el nombre de tu base de datos
collection = db['GPS_Data']  # Reemplaza 'tu_coleccion' con el nombre de tu colección en MongoDB

# Callback que se ejecuta cuando el cliente se conecta al broker
def on_connect(client, userdata, flags, rc):
    print("Conectado con el código de resultado: " + str(rc))

    # Suscribirse a un tema (topic) en el broker
    client.subscribe("GPS")

# Callback que se ejecuta cuando se recibe un mensaje en el tema suscrito
def on_message(client, userdata, msg):
    # Controla el mensaje recibido aquí
    json_object = json.loads(msg.payload.decode())
    #collection.insert_one(json_object)
    print(json_object)

# Configurar el cliente MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Establecer la dirección del broker y el puerto (por defecto es 1883)
# broker_address = "192.168.203.22"
broker_address = "192.168.2.107"

port = 1883

# Conectar al broker
client.connect(broker_address, port, 60)

# Mantener la conexión MQTT activa
client.loop_forever()
