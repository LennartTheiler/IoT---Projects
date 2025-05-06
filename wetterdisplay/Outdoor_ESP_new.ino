// Includes
#include <esp_now.h>
#include <HTTPClient.h>   
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>



// Struct von Daten die gesendet werden sollen
typedef struct struct_message {
  float TEMP;
  int HUM;
  int PRESS;
} struct_message;

struct_message myData;

esp_now_peer_info_t peerInfo;

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xEC, 0xDA, 0x3B, 0x51, 0x5D, 0xAC};

// Sensor-Objekte
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starte Sensorinitialisierung...");
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // I2C-Bus auf benutzerdefinierte Pins setzen: SDA = GPIO6, SCL = GPIO5
  Wire.begin(6, 5);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // AHT20 initialisieren
  if (!aht.begin()) {
    Serial.println("❌ AHT20 nicht gefunden!");
    //while (1) delay(10);
  } else {
    Serial.println("✅ AHT20 erkannt.");
  }

  // BMP280 initialisieren
  if (!bmp.begin(0x77)) {
    Serial.println("❌ BMP280 nicht gefunden!");
    //while (1) delay(10);
  } else {
    Serial.println("✅ BMP280 erkannt.");
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {

  // AHT20 auslesen
  sensors_event_t humidity, tempAHT;
  aht.getEvent(&humidity, &tempAHT);

  Serial.println("==================================");
  Serial.print("🌡️  AHT20 Temperatur: ");
  Serial.print(tempAHT.temperature);
  Serial.println(" °C");

  Serial.print("💧 AHT20 Luftfeuchtigkeit: ");
  Serial.print(humidity.relative_humidity);
  Serial.println(" %");

  Serial.print("🧭 BMP280 Luftdruck: ");
  Serial.print(bmp.readPressure() / 100.0F);
  Serial.println(" hPa");


  // Set values to send
  myData.TEMP = tempAHT.temperature;
  myData.HUM = humidity.relative_humidity;
  myData.PRESS = bmp.readPressure() / 100.0F;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
    Serial.println(myData.TEMP);
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}
