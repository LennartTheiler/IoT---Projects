# IoT LLM Umgebungsanalyse

Ein IoT-System zur visuellen Objekterkennung in Echtzeit mit einem ESP32-CAM Modul, einem MQTT-Trigger und einem Flask-Webserver. Erkennt Objekte auf aufgenommenen Bildern, wandelt die Ergebnisse in Sprache um und streamt Audio zurück an ein ESP32-Gerät.

## Projektidee

Das Projekt kombiniert Edge-Geräte, Cloud-Services und KI-basierte Bildanalyse. Ein ESP32 sendet einen Trigger über MQTT, ein Python-Server macht ein Foto, analysiert es mit einem Bild-KI-Modell, konvertiert das Ergebnis zu Sprache und spielt die Audiodatei über das ESP-Modul ab.

---

## Projektstruktur

```text
projekt/
├── main_L.py             
├── mqtt.py               
├── Kamera.py              
├── gTTS.py                
├── output.mp3             
├── output.wav            
├── templates/
│   └── index.html        
├── static/
│   ├── prompt.txt       
│   └── screenshots/
│       └── screenshot.jpg 
```

---

## Setup & Ausführung

### Voraussetzungen

- Python 3.8+
- Abhängigkeiten:
  ```bash
  pip install flask paho-mqtt opencv-python requests gtts pydub
  ```

- Ollama installiert (für Bildanalyse mit lokalem Modell)

### Projekt starten

1. **Flask-Server starten** (inkl. Kamera-Stream & API):
   ```bash
   python3 main_L.py
   ```

2. **ESP32-CAM konfigurieren**, um den Livestream unter z. B. `http://192.168.xxx.xxx:81/stream` verfügbar zu machen.

3. **ESP32-MQTT-Button** sendet Trigger an:
   ```
   Topic: kamera/trigger
   Broker: 192.168.xxx.xxx:1883
   ```

---

## Webinterface

- **Startseite**: `http://<Deine-IP>:5000`
- Funktionen:
  - Live-Stream der Kamera
  - Letzter Screenshot
  - Button zur manuellen Bildaufnahme
  - Automatische Aktualisierung des Analyse-Prompts
  - Sprachausgabe bei Klick oder Trigger

---

## Analyse & Verarbeitung

### Kamera.py

Verwendet das lokale `ollama`-Modell (z. B. gemma3:4b), um das Bild in eine Liste von Objekten (Nomen) umzuwandeln und in `prompt.txt` zu speichern.

### gTTS.py

Liest `prompt.txt`, generiert mit gTTS eine MP3-Datei und konvertiert diese in WAV.

### Flask-Endpunkte

- `/button_pressed` – Macht Screenshot & startet Analyse  
- `/get_prompt [GET]` – Gibt aktuellen Prompt zurück  
- `/get_prompt [POST]` – Startet Text-to-Speech & schickt WAV an ESP  
- `/video_feed` – Streamt Kamera-Livestream als MJPEG  

---

## Features

- MQTT-basierter Trigger (ESP32-kompatibel)  
- Live-Videostream via OpenCV  
- Screenshot per Button oder Trigger  
- Bildanalyse mit Ollama (lokales LLM)  
- Ausgabe als Sprache (gTTS → WAV)  
- Audio-Upload an ESP32 zur Wiedergabe  

---

## Beispielablauf

1. ESP32 sendet Trigger via MQTT  
2. Server speichert Screenshot vom Stream  
3. Kamera.py analysiert Bild (→ prompt.txt)  
4. gTTS erzeugt Sprachausgabe (output.wav)  
5. WAV-Datei wird an das ESP32-Modul gesendet  

---

## Hinweise

- Stelle sicher, dass das Ollama-Modell lokal verfügbar ist (`ollama run gemma3:4b`)  
- Passe IP-Adressen in `main_L.py`, `mqtt.py` und HTML ggf. an dein Netzwerk an  
- Achte auf Berechtigungen zum Schreiben in `static/` und `templates/`  

---
