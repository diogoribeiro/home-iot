#include <ESP8266WiFi.h>

//Include DHT11 lib
#include "DHT.h"
 
//Set DHT pin = D5
#define DHT_DATA_PIN 2
#define DHTTYPE DHT11

WiFiClient client;
DHT dht(DHT_DATA_PIN, DHTTYPE);

const char* ssid = ""; // your wireless network name (SSID)
const char* password = ""; // your Wi-Fi network password


// ThingSpeak Settings
const int channelID = ;
String writeAPIKey = ""; // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com";

const int postingInterval = 60 * 1000; // post data every 1 min.

unsigned long lastLoadMillis;
 
void setup() {
  Serial.begin(9600);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected");
}

bool isTimeToRead() {
  return !lastLoadMillis || ((millis() - lastLoadMillis)) >= postingInterval;
}

float readHumidity() {
  return dht.readHumidity();
}

float readTemperature() {
  return dht.readTemperature();
}

void collectData() {
    if (client.connect(server, 80)) {
    
    float humidity = readHumidity();
    float temperature = readTemperature();
 
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Erro ao ler o sensor!");
      return;
    }

    String body = "field1=";
           body += String(temperature);
           body += "&field2=";
           body += String(humidity);
    
    Serial.print("temp: ");
    Serial.println(temperature); 

    Serial.print("humidity: ");
    Serial.println(humidity); 

    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: ESP8266 (nothans)/1.0");
    client.println("Connection: close");
    client.println("X-THINGSPEAKAPIKEY: " + writeAPIKey);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(body.length()));
    client.println("");
    client.print(body);
  }

  client.stop();
  lastLoadMillis = millis();
}
 
void loop() {
  if (isTimeToRead()) {
    collectData();
  }
}
