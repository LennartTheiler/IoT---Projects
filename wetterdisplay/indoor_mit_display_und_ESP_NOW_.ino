#include <esp_now.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
//----------------------------------------------------------------------------------------------------
// Defines und Globale Variablen
//----------------------------------------------------------------------------------------------------
typedef struct struct_message {
  float TEMP;
  int HUM;
  int PRESS;
} struct_message;

struct_message myData;

// Globale Variablen für Sensordaten
float temp = 0.0;
int humidity = 0;
int pressure = 0;

// API-Daten
float temp_api = 0.0;

// Pinbelegung für das Display
#define TFT_CS    10
#define TFT_DC    9
#define TFT_RST   46

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Wetter-API-Zugang
const char *apiKey = "fd66e59178331891c08a6a7ceee9981c";
const char *city = "Karlsruhe,de";

//----------------------------------------------------------------------------------------------------
// ESP-NOW Callback bei Datenempfang
//----------------------------------------------------------------------------------------------------
void OnDataRecv(const esp_now_recv_info *recv_info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  temp = myData.TEMP;
  humidity = myData.HUM;
  pressure = myData.PRESS;

  Serial.print("Bytes received: ");
  Serial.println(len);

  Serial.print("Temperatur: ");
  Serial.println(temp);
  Serial.print("Luftfeuchtigkeit: ");
  Serial.println(humidity);
  Serial.print("Luftdruck: ");
  Serial.println(pressure);
  Serial.println();

  show_on_display();  // Anzeige aktualisieren
}
//----------------------------------------------------------------------------------------------------
// Anzeige der Werte + Delta + Empfehlung
//----------------------------------------------------------------------------------------------------
void show_on_display() {
  float delta_temp = abs(temp_api - temp);

  tft.fillRect(0, 0, tft.width(), 140, ILI9341_BLACK); // Anzeige leeren

  tft.setCursor(10, 10);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);

  tft.print("Temp (ESP): ");
  tft.print(temp);
  tft.println(" C");

  tft.setCursor(10, 30);
  tft.print("Temp (API): ");
  tft.print(temp_api);
  tft.println(" C");

  tft.setCursor(10, 50);
  tft.print("Delta: ");
  tft.print(delta_temp, 2);
  tft.println(" C");

  tft.setCursor(10, 70);
  tft.print("Hum: ");
  tft.print(humidity);
  tft.println(" %");

  tft.setCursor(10, 90);
  tft.print("Pres: ");
  tft.print(pressure);
  tft.println(" hPa");

  clothing_recommendation(); // Kleidungstipp anzeigen
}

// Kleidungsempfehlung abhängig vom Wetter
void clothing_recommendation() {
  String recommendation = "";

  if (temp < 10) {
    recommendation = "Winterjacke!";
  } else if (humidity > 70 && pressure < 1000) {
    recommendation = "Regenjacke!";
  } else if (temp > 25) {
    recommendation = "Strandklamotten!";
  } else {
    recommendation = "Normale Kleidung.";
  }

  // Text auf dem Display anzeigen
  tft.fillRect(0, 120, tft.width(), 30, ILI9341_BLACK);
  tft.setCursor(10, 120);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  tft.print("Tipp: ");
  tft.print(recommendation);

  Serial.print("Kleidungstipp: ");
  Serial.println(recommendation);
}
//----------------------------------------------------------------------------------------------------
// API-Abfrage (OpenWeatherMap)
//----------------------------------------------------------------------------------------------------
void getWeatherAPI() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "&units=metric&appid=" + String(apiKey);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        temp_api = doc["main"]["temp"];
        Serial.print("API Temp: ");
        Serial.println(temp_api);
      } else {
        Serial.println("Fehler beim JSON-Parsen");
      }
    } else {
      Serial.print("HTTP Fehler: ");
      Serial.println(http.errorToString(httpCode));
    }
    http.end();
  }
}

// Funktion für LED-Feedback (optional anpassen)
void rgbLedWrite(int pin, int r, int g, int b) {
  // Dummyfunktion – ggf. anpassen
}
//----------------------------------------------------------------------------------------------------
// WLAN-Verbindung
//----------------------------------------------------------------------------------------------------
const char* ssid = "OpenWrt";
const char* password = "FU12Labor";

void connect_to_wifi(){
  Serial.print("\nVerbinde mit WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Verbunden!");
  connected();
}

void connected(){
  rgbLedWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0);
}

void disconnected(){
  Serial.println("WiFi Disconnected... Versuche neu zu verbinden.");
  rgbLedWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0);
  connect_to_wifi();
}
//----------------------------------------------------------------------------------------------------
// Setup
//----------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("ILI9341 Test startet...");

  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(ILI9341_BLACK);

  tft.setCursor(20, 20);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(3);
  tft.println("Display OK!");

  rgbLedWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0);

  connect_to_wifi();
  delay(1000);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Fehler");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  tft.fillScreen(ILI9341_BLUE); delay(500);
  tft.fillScreen(ILI9341_RED); delay(500);
  tft.fillScreen(ILI9341_BLACK);

  getWeatherAPI(); // Erste API-Daten laden
}
//----------------------------------------------------------------------------------------------------
// Loop
//----------------------------------------------------------------------------------------------------
unsigned long lastApiUpdate = 0;
const unsigned long apiInterval = 60000; // Alle 60 Sekunden

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    disconnected();
    delay(10000);
  }

  // API-Daten regelmäßig abrufen
  unsigned long now = millis();
  if (now - lastApiUpdate > apiInterval) {
    getWeatherAPI();
    lastApiUpdate = now;
  }

  delay(6000);
}
