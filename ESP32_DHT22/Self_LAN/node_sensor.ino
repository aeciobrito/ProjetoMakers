#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// Replace with your access point credentials
const char* ssid = "ESP32-AP";
const char* password = "123456789";

// Replace with the IP address of your central server ESP32
const char* serverName = "http://192.168.4.1/post-data"; // Use the default AP IP address

// DHT sensor configuration
#define DHTPIN 4 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Room identifier
const char* location = "Escritorio";

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize the DHT sensor
  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;

    // Read data from the DHT sensor
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Check if readings failed and try again
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Prepare JSON payload
    String jsonPayload = "{\"location\":\"" + String(location) + "\",\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";

    http.begin(serverName); // Specify the URL
    http.addHeader("Content-Type", "application/json"); // Specify content-type header

    int httpResponseCode = http.POST(jsonPayload); // Send the request

    // Print response
    if (httpResponseCode > 0) {
      String response = http.getString(); // Get the response to the request
      Serial.println(httpResponseCode); // Print return code
      Serial.println(response); // Print request answer
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Free resources

  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(2000); // Send a request every 2 seconds
}
