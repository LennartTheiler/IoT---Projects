from flask import Flask, Response, request, render_template, send_file
import cv2
import socket
import os
import subprocess
import threading
import requests
from mqtt import MQTTClient
from gTTS import TTS

from pydub import AudioSegment


app = Flask(__name__)
app.config['TEMPLATES_AUTO_RELOAD'] = True
last_frame = None

SCREENSHOT_DIR = '/static/screenshots'
PROMPT_FILE   = '/static/prompt.txt'

stream_url = "http://192.168.xxx.xxx:81/stream"
cap = cv2.VideoCapture(stream_url)

def generate_frames():
    """
    Liest kontinuierlich Frames vom Kamera-Stream, rotiert sie und gibt sie als MJPEG-Stream aus.
    """
    global last_frame
    while True:
        success, frame = cap.read()
        if not success:
            break
        frame = cv2.rotate(frame, cv2.ROTATE_180)
        ret, buffer = cv2.imencode('.jpg', frame)
        last_frame = buffer.tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + last_frame + b'\r\n')


@app.route('/')
def index():
    """
    Lädt das HTML-Frontend und gibt die Startseite zurück.
    """
    print("starting html-file...")
    server_ip = request.host
    return render_template('index.html', server_ip=server_ip)


@app.route('/button_pressed', methods=['POST'])
def button_pressed():
    """
    Speichert den aktuellen Frame als Screenshot und ruft Kamera.py zur Bildanalyse auf.
    """
    print("Button wurde gedrückt!")
    if last_frame is None:
        print("Kein Bild verfügbar.")
        return "Kein Bild verfügbar."

    os.makedirs(SCREENSHOT_DIR, exist_ok=True)
    screenshot_path = os.path.join(SCREENSHOT_DIR, 'screenshot.jpg')
    with open(screenshot_path, 'wb') as f:
        f.write(last_frame)
    print("Screenshot gespeichert:", screenshot_path)

    try:
        result = subprocess.run(
            ["python3", "/home/laboradmin/Projekt_2/Kamera.py"],
            check=True,
            capture_output=True,
            text=True
        )
        print("Kamera.py beendet. Ausgabe:\n", result.stdout)
    except subprocess.CalledProcessError as e:
        print("Kamera.py schlug fehl:\n", e.stderr)
    except Exception as e:
        print(f"Unerwarteter Fehler beim Ausführen von Kamera.py: {e}")

    return "Button wurde gedrückt!"

@app.route('/get_prompt', methods=['GET', 'POST'])
def get_prompt_first():
    """
    Gibt den Analyse-Prompt zurück (GET) oder erzeugt Sprachausgabe und sendet WAV-Datei an ESP32 (POST).
    """
    if request.method == 'GET':
        if os.path.exists(PROMPT_FILE):
            with open(PROMPT_FILE, 'r', encoding='utf-8') as f:
                return f.read()
        else:
            return "Noch keine Analyse vorhanden."
    elif request.method == 'POST':
        create_wave()
        send_wav_to_esp()
        return "Audio erzeugt", 200
    

@app.route('/video_feed')
def video_feed():
    """
    Gibt den MJPEG-Videostream des Kamerafeeds zurück.
    """
    return Response(generate_frames(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

def send_wav_to_esp():
    """
    Sendet die WAV-Datei per HTTP POST an das ESP32-Modul.
    """
    url = "http://192.168.xxx.xxx/upload_audio"  # ESP-IP
    mp3_path = "./output.wav"

    if os.path.exists(mp3_path):
        with open(mp3_path, 'rb') as f:
            files = {'file': ('output.wav', f, 'audio/mpeg')}
            try:
                response = requests.post(url, files=files)
                print("ESP-Antwort:", response.status_code, response.text)
            except Exception as e:
                print("Fehler beim Senden an ESP:", e)

def start_mqtt_client():
    """
    Startet den MQTT-Client in einem eigenen Thread, um auf Trigger vom ESP32 zu reagieren.
    """
    mqtt_client = MQTTClient()
    mqtt_client.start()


def create_wave():
    """
    Wandelt den Analyse-Text in eine WAV-Audiodatei um und komprimiert diese für die Übertragung.
    """
    tts = TTS()
    tts.text_to_speech()
    print("output.wav wurde erstellt")

    # Kompression: 8000 Hz, Mono, 8 Bit
    try:
        audio = AudioSegment.from_wav("output.wav")
        compressed = audio.set_frame_rate(44100).set_channels(2).set_sample_width(2)
        compressed.export("output.wav", format="wav")
        print("output.wav wurde auf 8000Hz, Mono, 8bit komprimiert.")
    except Exception as e:
        print("❌ Fehler bei WAV-Komprimierung:", e)



if __name__ == "__main__":
    threading.Thread(target=start_mqtt_client, daemon=True).start()
    app.run(host='0.0.0.0', port=5000)
