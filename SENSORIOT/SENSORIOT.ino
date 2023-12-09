#include "TRIGGER_WIFI_ESP32.h"
#include "TRIGGER_GOOGLESHEETS_ESP32.h"
#include "DHT.h"
#include <HTTPClient.h> 

#define DHT11PIN 5
#define SOIL_MOISTURE_PIN 34
#define BLUE_LED_PIN 2 // Ganti dengan pin GPIO yang sesuai

DHT dht(DHT11PIN, DHT11);

char column_name_in_sheets[][20] = {"value1", "value2", "value3"};
String Sheets_GAS_ID = "AKfycbwkQuPhygjaRH-7tL4NG552GosExYgnJ4-twkqjSowcuiyrOAAQ3k1A8nKbEs5dz4UE";
int No_of_Parameters = 3;

const char* ssid = "Fern"; // Ganti dengan SSID jaringan Wi-Fi Anda
const char* password = "1234567890e"; // Ganti dengan kata sandi jaringan Wi-Fi Anda

unsigned long loopCount = 0; // Counter loop

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Atur pin LED biru sebagai output
  pinMode(BLUE_LED_PIN, OUTPUT);

  WIFI_Connect(ssid, password);
  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters);
  delay(5000);
}

void loop() {
  float humi = dht.readHumidity();
  float temp = dht.readTemperature();
  delay(2000);
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
  float soilMoisturePercent = map(soilMoistureValue, 0, 4095, 100, 0);


  if (loopCount >= 240) {
    Serial.println("Program telah berjalan 130 kali loop. Berhenti.");
    analogWrite(BLUE_LED_PIN, 100); // Nyalakan lampu biru
    while (1) {
      // Berhenti dengan loop tak berujung
    }
  }

    if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi terputus. Mencoba menyambungkan kembali...");

    // Coba sambungkan WiFi lagi
    WiFi.begin(ssid, password);

    // Tunggu sampai terhubung ke Wi-Fi
    while (WiFi.status() != WL_CONNECTED) {
      Serial.println("...");
      analogWrite(BLUE_LED_PIN, 100); // Nyalakan lampu biru
      delay(1000);
    }

    Serial.println("Terhubung ke Wi-Fi kembali.");
    delay(10000);
  }

  

  // Buat objek HTTPClient
  HTTPClient http;

  // Buat URL dengan parameter
  String url = "https://script.google.com/macros/s/" + Sheets_GAS_ID + "/exec";
  url += "?value1=" + String(soilMoisturePercent);
  url += "&value2=" + String(temp);
  url += "&value3=" + String(humi);

  // Kirim permintaan HTTP GET
  http.begin(url);

  int httpCode = http.GET(); // Eksekusi permintaan GET

  http.end(); // Akhiri sesi HTTP

  if (httpCode == 200) {
    Serial.println("Gagal mengirim data ke Google Sheets.");
    Serial.print(soilMoisturePercent);
    Serial.print("   ");
    Serial.print(temp);
    Serial.print("   ");
    Serial.println(humi);
  } else {
    Serial.println("Data terkirim ke Google Sheets dengan sukses.");
    Serial.print(soilMoisturePercent);
    Serial.print("   ");
    Serial.print(temp);
    Serial.print("   ");
    Serial.println(humi);
  }

  // Tambahkan jeda waktu sebelum mengirim data berikutnya
  delay(60000);

  // Increment counter
  loopCount++;
}