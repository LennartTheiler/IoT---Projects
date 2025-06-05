# 📸 IoT Cloud Bilderkennung mit MQTT, Flask und ESP32

Ein IoT-System zur visuellen Objekterkennung in Echtzeit mit einem ESP32-CAM Modul, einem MQTT-Trigger und einem Flask-Webserver. Erkennt Objekte auf aufgenommenen Bildern, wandelt die Ergebnisse in Sprache um und streamt Audio zurück an ein ESP32-Gerät.

## 🧠 Projektidee

Das Projekt kombiniert Edge-Geräte, Cloud-Services und KI-basierte Bildanalyse. Ein ESP32 sendet einen Trigger über MQTT, ein Python-Server macht ein Foto, analysiert es mit einem Bild-KI-Modell, konvertiert das Ergebnis zu Sprache und spielt die Audiodatei über das ESP-Modul ab.

---

## 🗂️ Projektstruktur

```text
projekt/
├── main_L.py              # Haupt-Flask-Server inkl. Kamera-Streaming, API und MQTT-Thread
├── mqtt.py                # MQTT-Client, der auf ESP32-Trigger hört
├── Kamera.py              # Führt Bildanalyse mit Ollama-Modell durch
├── gTTS.py                # Text-to-Speech mit gTTS + Konvertierung zu WAV
├── output.mp3             # MP3-Audioausgabe (Zwischenschritt)
├── output.wav             # WAV-Datei für den ESP32
├── templates/
│   └── index.html         # HTML-Frontend zur Anzeige von Kamera-Stream und Screenshot
├── static/
│   ├── prompt.txt         # Textausgabe der KI-Bildbeschreibung
│   └── screenshots/
│       └── screenshot.jpg # Letztes aufgenommenes Bild

```

