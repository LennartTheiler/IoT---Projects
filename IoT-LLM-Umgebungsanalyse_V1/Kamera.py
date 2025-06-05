import ollama
import os

# Pfad zu Datei und Bild
PROMPT_FILE = '/home/laboradmin/Projekt_2/static/prompt.txt'
IMAGE_PATH  = '/home/laboradmin/Projekt_2/static/screenshots/screenshot.jpg'

# Sicherstellen, dass das Bild existiert
if not os.path.exists(IMAGE_PATH):
    raise FileNotFoundError(f"Bild nicht gefunden unter: {IMAGE_PATH}")

# Bild analysieren mit Ollama
try:
    res = ollama.chat(
        model="gemma3:4b",
        messages=[{
            'role': 'user',
            'content': (
                'Analysiere dieses Bild. Gib nur eine Liste der erkannten Objekte als einzelne Nomen zurück. Keine Verben, keine Adjektive, keine Satzzeichen, keine Sonderzeichen. Nur die Nomen, durch Leerzeichen getrennt.'
            ),
            'images': [IMAGE_PATH]
        }]
    )
    beschreibung = res['message']['content'].strip()

except Exception as e:
    beschreibung = "Fehler bei Analyse"
    print(f"Analyse fehlgeschlagen: {e}")

# Ergebnis in Datei schreiben
try:
    os.makedirs(os.path.dirname(PROMPT_FILE), exist_ok=True)
    with open(PROMPT_FILE, 'w', encoding='utf-8') as f:
        f.write(beschreibung)
        f.flush()
except Exception as e:
    print(f"Fehler beim Schreiben der Datei: {e}")
