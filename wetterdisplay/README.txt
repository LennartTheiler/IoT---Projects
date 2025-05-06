--- Projektbeschreibung ---

Dieses Projekt besteht aus zwei ESP32-basierten Modulen, die über das ESP-NOW-Protokoll kommunizieren. Ziel ist es, Sensordaten (z. B. Temperatur, Luftfeuchtigkeit, o. ä.) drahtlos von einem Outdoor-Modul an ein Indoor-Modul zu übertragen, wo sie auf einem OLED-Display dargestellt werden. Ein drittes Skript dient zur einfachen Ermittlung der MAC-Adresse eines ESP32, um die Kommunikation korrekt zu konfigurieren.


--- Projektstruktur ---

1. Outdoor_ESP_new.ino
Funktion: Sendet regelmäßig Sensordaten (Dummy-Daten oder echte Messwerte) über ESP-NOW.

Rolle: Sender.

Besonderheiten:

Verwendet die MAC-Adresse des Empfängers, um gezielt zu senden.

Nutzt eine Struktur (struct) zur Datenübertragung.

2. indoor_mit_display_und_ESP_NOW_.ino
Funktion: Empfängt die Daten über ESP-NOW und zeigt sie auf einem OLED-Display an.

Rolle: Empfänger und Anzeigeeinheit.

Besonderheiten:

Setzt ein I²C-Display (z. B. SSD1306) zur Visualisierung ein.

Zeigt z. B. Temperaturwerte, Signalstärke, Zeitstempel o. ä.

3. MacAdresseKriegen.ino
Funktion: Gibt die MAC-Adresse des aktuell angeschlossenen ESP32 im seriellen Monitor aus.

Rolle: Hilfsskript.

Verwendung: Zum Eintragen der MAC-Adresse des Empfängers im Sender-Sketch.