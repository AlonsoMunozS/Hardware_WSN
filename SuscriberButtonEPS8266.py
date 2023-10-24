import paho.mqtt.client as mqtt
import json
import datetime

# Conexión a la base de datos MongoDB
# Callback de conexión MQTT
def on_connect(client, userdata, flags, rc):
    print("Conectado con el código de resultado: " + str(rc))
    client.subscribe("Button")

# Callback de recepción de mensajes MQTT
def on_message(client, userdata, msg):
    current_datetime = datetime.datetime.now()
    formatted_date = current_datetime.strftime("%d / %m / %Y")
    formatted_time = current_datetime.strftime("%H : %M : %S")
    
    json_object_recibed = json.loads(msg.payload.decode())
    json_object_mongo = {
        "used": json_object_recibed["used"],
        "fecha": formatted_date,
        "hora": formatted_time
    }
    
    print(json_object_mongo)

# Configuración y conexión al broker MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

broker_address = "192.168.2.107"
port = 1883

client.connect(broker_address, port, 60)

# Mantener la conexión MQTT activa
client.loop_forever()
