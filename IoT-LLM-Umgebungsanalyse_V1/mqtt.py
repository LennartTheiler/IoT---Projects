import paho.mqtt.client as mqtt
import requests

class MQTTClient:

    def __init__(self):
        self.broker_ip = "192.168.179.244"
        self.broker_port = 1883
        self.topic = "kamera/trigger"
        self.FLASK_Endpoint = "http://192.168.179.244:5000/button_pressed"
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message

    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            print("MQTT-Client erfolgreich verbunden")
            client.subscribe(self.topic)
        else:
            print("Verbindung fehlgeschlagen, Code:", rc)

    def on_message(self, client, userdata, message):
        print(f"Nachricht empfangen: `{message.payload.decode()}` vom Topic `{message.topic}`")
        self.send_post_to_flask()

    def send_post_to_flask(self):
        try:
            response = requests.post(self.FLASK_Endpoint)
            print(f"POST gesendet an Flask-Server. Antwortstatus: {response.status_code}")
        except requests.RequestException as e:
            print("Fehler beim Senden der Anfrage an den Flask-Server:", e)

    def start(self):
        self.client.connect(self.broker_ip, self.broker_port)
        self.client.loop_forever()

if __name__ == "__main__":
    mqtt_client = MQTTClient()
    mqtt_client.start()