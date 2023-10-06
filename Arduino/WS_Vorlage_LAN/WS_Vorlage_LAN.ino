#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <Ethernet.h>
#include <Dns.h>

#define DHTPIN 2
#define DHTTYPE DHT22

byte mac[] = { 0x02, 0xEA, 0xEF, 0x53, 0x54, 0x92 };

IPAddress server_ip;

EthernetClient client;
MySQL_Connection conn((Client *)&client);
DNSClient dns_client;

MySQL_Cursor *cur;
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

char STATIONNAME[] = "Arduino_LAN_X";
char USRNAME[] = "<USERNAME>";
char PWD[] = "<PASSWORD>";
char DBNAME[] = "<DBNAME>";
char HOSTNAME[] = "<SERVERURL>";

const char insertdata[] = "CALL `<DBNAME>`.`new_Weatherdata`('ARD_WS_Name', %i.%02d, %i.%02d, %s)";

char *buf = new char[sizeof(insertdata) / sizeof(char) + 50];

void ConnectDB() {
  Serial.println(F("Connecting SQL..."));
  dns_client.getHostByName(HOSTNAME, server_ip);
  if (!conn.connect(server_ip, 3306, USRNAME, PWD)) {
    Serial.println(F("Connection failed."));
  } else
    Serial.println("Connection established.");
}


void setup() {
  Serial.begin(9600);
  pinMode(DHTPIN, INPUT);
  dht.begin();
  while (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP085 sensor, check wiring!"));
    delay(1000);
  }
  Serial.println("Start connecting Ethernet.");
  while (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    delay(1000);
  }
  dns_client.begin(Ethernet.dnsServerIP());
  Serial.println("Connection established.");
}

void loop() {
  
  ConnectDB();
float h = dht.readHumidity();  
  float t = (dht.readTemperature()+bmp.readTemperature())/2.0;
  float p = (float)bmp.readPressure();
  char buf2[7];
  dtostrf(p,7,2,buf2);
  sprintf(buf, insertdata, (int)h, (int)(h * 100) % 100, (int)t, (int)(t * 100) % 100, buf2);
  // Serial.println(buf);
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(buf);
  delete cur_mem;

  if (conn.connected()) {
    conn.close();
  }
  Ethernet.maintain();
  delay(60000);
}
