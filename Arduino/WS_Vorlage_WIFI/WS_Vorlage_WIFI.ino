#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <MySQL_Generic.h>

#define DHTPIN 2
#define DHTTYPE DHT22

#define SSIDNAME "<SSID>"
#define SSID_PASS "<SSIDPASSWORD>"

IPAddress server_ip;

MySQL_Connection conn((Client *)&client);

MySQL_Query *query_mem;
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

char STATIONNAME[] = "NodeMCU_X";
char USRNAME[] = "<USERNAME>";
char PWD[] = "<PASSWORD>";
char DBNAME[] = "<DBNAME>";
char HOSTNAME[] = "<SERVERURL>";

const char insertdata[] = "CALL `<DBNAME>`.`new_Weatherdata`('ARD_WS_X', %d.%02d,%d.%02d,%d.%02d)";

char *buf = new char[sizeof(insertdata) / sizeof(char) + 50];

void ConnectWifi() {
  if (WiFi.status() == WL_CONNECTED)
    return;
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.hostname(STATIONNAME);
    // Serial.print(F("Attempting to connect to WPA SSID: "));
    // Serial.println(SSIDNAME);
    WiFi.begin(SSIDNAME, SSID_PASS);
    delay(5000);
  }
  //  if (WiFi.status() == WL_CONNECTED)
  //  {
    
    // Serial.print(F("Connected to WPA SSID: "));
    // Serial.println(SSIDNAME);
  //  }
}

void ConnectDB() {
  if (conn.connected())
    return;
  // Serial.println(F("Connecting..."));
  WiFi.hostByName(HOSTNAME, server_ip);
  if (!conn.connect(server_ip, 3306, USRNAME, PWD)) {
    // Serial.println(F("Connection failed."));
  }
}


void setup() {
  Serial.begin(9600);
  pinMode(DHTPIN, INPUT);
  dht.begin();
  while (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP085 sensor, check wiring!"));
    delay(1000);
  }
  ConnectWifi();
  return;
}

void loop() {
  float h = dht.readHumidity();  
  float t = (dht.readTemperature()+bmp.readTemperature())/2.0;
  long p = bmp.readPressure();
  // Serial.print("DHT 22 Luftfeuchtigkeit: ");
  // Serial.print(h);      // Ausgeben der Luftfeuchtigkeit
  // Serial.print("%\t");  // Tabulator
  // Serial.print("Temperatur: ");
  // Serial.print(t);  // Ausgeben der Temperatur
  // Serial.print("C");
  // Serial.print("%\t");  // Tabulator
  // Serial.print("Luftdruck: ");
  // Serial.print(p / 100);
  // Serial.println(" hPa");
  ConnectDB();
  sprintf(buf, insertdata, (int)h, (int)(h * 100) % 100, (int)t, (int)(t * 100) % 100, (int)p, (int)(p * 100) % 100);
  MySQL_Query query_mem = MySQL_Query(&conn);
  MYSQL_DISPLAY(buf);
    
    if ( !query_mem.execute(buf) )
    {
      MYSQL_DISPLAY("Insert error");
    }
    else
    {
      MYSQL_DISPLAY("Data Inserted.");
    }
  conn.close();
  delay(60000);
  return;
}
