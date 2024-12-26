
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define DHTPIN 2       // Pin für den DHT22-Sensor
#define MQ135_PIN A0   // Pin für den MQ-135 Gassensor
#define DHTTYPE DHT22  // DHT 22 (AM2302)
#define DEBUG 0        // 1 für serielle Debug-Ausgabe, 0 für keine Ausgabe

#define SSIDNAME "SSIDNAME"
#define SSID_PASS "PASSWORT0815"

DHT dht(DHTPIN, DHTTYPE);  // Initialisierung des DHT22
Adafruit_BMP085 bmp;       // Initialisierung des BMP085

WiFiServer server(5000);  // Der WiFi-Server läuft auf Port 80 (HTTP)
WiFiClient client;      // WiFiClient-Objekt zum Handling der Client-Verbindungen

char STATIONNAME[] = "WetterstationXX";
String command = "";

void ConnectWifi() {
  if (WiFi.status() == WL_CONNECTED)
    return;
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.hostname(STATIONNAME);
    Serial.print(F("Attempting to connect to WPA SSID: "));
    Serial.println(SSIDNAME);
    WiFi.begin(SSIDNAME, SSID_PASS);
    delay(5000);
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print(F("Connected to WPA SSID: "));
    Serial.println(SSIDNAME);
  }
}

void setup() {

  pinMode(DHTPIN, INPUT);
  pinMode(MQ135_PIN, INPUT);

  dht.begin();
  while (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP085 sensor, check wiring!"));
    delay(1000);
  }

  if (DEBUG) {
    Serial.begin(9600);
  }
  ConnectWifi();

  server.begin();  // WiFiServer starten
}

void loop() {
  // Überprüfen, ob ein neuer Client eine Verbindung hergestellt hat
  client = server.available();
  if (client) {
    if (DEBUG) {
      Serial.println(F("Client connected."));
    }

    while (client.connected()) {
      // Lese die eingehenden Daten vom Client
      if (client.available()) {
        char c = client.read();
        command += c;
          command.trim();  // Entferne Leerzeichen
         if (command.startsWith("<RA>") && command.endsWith("</RA>")) {
            handleReadAllCommand(client);
          command = "";  // Befehl zurücksetzen
          }

        
      }
    }

    // Verbindung trennen, wenn der Client fertig ist
    client.stop();
    command = "";
    if (DEBUG) {
      Serial.println(F("Client disconnected."));
    }
  }

  // Debug-Modus: Serielle Schnittstelle verwenden
  if (DEBUG && Serial) {
    while (Serial.available()) {
      command = Serial.readStringUntil('\n');
      command.trim();
      if (command.length() == 0) continue;
      if (command.startsWith("<RA>") && command.endsWith("</RA>")) {
        handleReadAllCommand(Serial);
      } else {
        Serial.print("<F>");
        Serial.print(command);
        Serial.println("</F>");
      }
    }
  }
}



void handleReadAllCommand(Print &output) {
  // Alle Werte als eine Abfrage, z.B. "<RA>"
  char buffer[200];
  snprintf(buffer, sizeof(buffer),
           "|<RA>T=%.2f;H=%.2f;P=%.2f;MQ135=%d;OK</RA>|",
           (dht.readTemperature() + bmp.readTemperature()) / 2,  // Durchschnittstemperatur
           dht.readHumidity(),                                   // Luftfeuchtigkeit DHT22
           bmp.readPressure() / 100.0F,                          // Luftdruck BMP085 in hPa
           analogRead(MQ135_PIN));                               // MQ-135 Gassensorwert
  output.println(buffer);
}
