import paho.mqtt.client as mqtt
import requests

class MQTTClient:
    """
    MQTT-Client, der auf Nachrichten im Topic 'kamera/trigger' hört und bei Empfang
    eine POST-Anfrage an einen Flask-Server sendet.
    """

    def __init__(self):
        """
        Initialisiert den MQTT-Client mit Broker-IP, Port, Topic und Flask-Endpoint.
        Setzt die Callback-Funktionen on_connect und on_message.
        """
        self.broker_ip = "192.168.xxx.xxx"
        self.broker_port = 1883
        self.topic = "kamera/trigger"
        self.FLASK_Endpoint = "http://192.168.xxx.xxx:5000/button_pressed"
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message

    def on_connect(self, client, userdata, flags, rc):
        """
        Callback, wenn Verbindung mit dem MQTT-Broker hergestellt wurde.
        Abonniert das definierte Topic, falls rc == 0 (Erfolg).
        """
        if rc == 0:
            print("MQTT-Client erfolgreich verbunden")
            client.subscribe(self.topic)
        else:
            print("Verbindung fehlgeschlagen, Code:", rc)

    def on_message(self, client, userdata, message):
        """
        Callback, wenn eine Nachricht im abonnierten Topic empfangen wurde.
        Loggt die Payload und sendet anschließend eine POST-Anfrage an den Flask-Server.
        """
        print(f"Nachricht empfangen: `{message.payload.decode()}` vom Topic `{message.topic}`")
        self.send_post_to_flask()

    def send_post_to_flask(self):
        """
        Sendet eine HTTP-POST-Anfrage an den Flask-Endpoint, um den Button-Pressed-Handler auszulösen.
        Gibt den HTTP-Statuscode aus oder einen Fehler, falls die Anfrage fehlschlägt.
        """
        try:
            response = requests.post(self.FLASK_Endpoint)
            print(f"POST gesendet an Flask-Server. Antwortstatus: {response.status_code}")
        except requests.RequestException as e:
            print("Fehler beim Senden der Anfrage an den Flask-Server:", e)

    def start(self):
        """
        Verbindet sich mit dem MQTT-Broker und startet die Endlosschleife (loop_forever),
        um kontinuierlich auf eingehende Nachrichten zu hören.
        """
        self.client.connect(self.broker_ip, self.broker_port)
        self.client.loop_forever()

if __name__ == "__main__":
    mqtt_client = MQTTClient()
    mqtt_client.start()
