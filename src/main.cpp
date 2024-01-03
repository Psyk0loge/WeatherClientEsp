#include "WiFi.h"
#include <HTTPClient.h>
#include "DHT.h"
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

#define DHTPIN 4     // Digital pin connected to the DHT sensor

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  300   
DHT dht(DHTPIN, DHTTYPE);
const String SERVER_NAME = "https://192.168.178.70/upload";
const String ssid = "SSidName";
const String password = "passwordString";

void setup() {
  
}

void loop() {
  // Wait a few seconds between measurements.
  Serial.begin(9600);
  delay(1000); //Take some time to open up the Serial Monitor
  Serial.println("DHTxx test!");
  WiFi.mode(WIFI_STA); //OptionalESP Async WebServer
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");
  delay(1000);
  dht.begin();
  if(WiFi.status() == WL_CONNECTED){
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);
    Serial.println(f);
    Serial.println(t);
    Serial.println(h);
        HTTPClient http;
        // http.begin(SERVER_NAME.c_str());
        // http.POST()
        StaticJsonDocument<200> jsonDoc;
        jsonDoc["temperature"] = t; // Setzen Sie die tatsächliche Temperatur
        jsonDoc["timeStamp"] = "string"; // Setzen Sie den tatsächlichen Zeitstempel
        jsonDoc["humidity"] = h; // Setzen Sie die tatsächliche Luftfeuchtigkeit
        jsonDoc["pressure"] = 0; // Setzen Sie den tatsächlichen Druck
        jsonDoc["passphrase"] = "espReading";
        String requestBody;
        serializeJson(jsonDoc, requestBody);

        http.begin(SERVER_NAME);
        http.addHeader("Content-Type", "application/json");

        // POST-Anfrage senden
        int httpResponseCode = http.POST(requestBody);
        if(httpResponseCode > 0){
          String response = http.getString();
          Serial.println(httpResponseCode);
          Serial.println(response);
        }else{
          Serial.print("Error on sending POST: ");
          Serial.println(httpResponseCode);
          Serial.println(http.errorToString(httpResponseCode)); // Gibt den Fehler als String aus
        }

        http.end();
        // Check if any reads failed and exit early (to try again).
        if (isnan(h) || isnan(t) || isnan(f)) {
          Serial.println(F("Failed to read from DHT sensor!"));
          return;
        }
        Serial.println("Going to sleep now");
        delay(1000);
        Serial.flush(); 
        esp_deep_sleep_start();
  }else{
    Serial.println("Nicht verbunden mit WiFi");
  }
  


}