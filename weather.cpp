#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include "DHT.h"

// Wi-Fi credentials
#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASSWORD "Your_WiFi_Password"

// Adafruit IO credentials
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883 // MQTT port
#define AIO_USERNAME "Your_Adafruit_IO_Username"
#define AIO_KEY "Your_Adafruit_IO_Key"

// DHT11 Configuration
#define DHTPIN 4         // Pin where the DHT11 is connected
#define DHTTYPE DHT11    // Define the DHT sensor type
DHT dht(DHTPIN, DHTTYPE);

// ESP32 Wi-Fi client and MQTT setup
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// MQTT Feeds for temperature and humidity
Adafruit_MQTT_Publish tempFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish humidFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
}

void connectToMQTT() {
  while (mqtt.connected() == false) {
    Serial.print("Connecting to MQTT...");
    if (mqtt.connect()) {
      Serial.println("Connected!");
    } else {
      Serial.print("Failed, retrying in 5 seconds. Error: ");
      Serial.println(mqtt.connectErrorString(mqtt.connectError()));
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectToWiFi();
}

void loop() {
  connectToMQTT();

  // Read temperature and humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Publish data to Adafruit IO
  if (tempFeed.publish(temperature)) {
    Serial.print("Temperature published: ");
    Serial.println(temperature);
  } else {
    Serial.println("Failed to publish temperature.");
  }

  if (humidFeed.publish(humidity)) {
    Serial.print("Humidity published: ");
    Serial.println(humidity);
  } else {
    Serial.println("Failed to publish humidity.");
  }

  delay(2000); // Send data every 2 seconds
}