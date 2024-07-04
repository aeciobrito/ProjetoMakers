#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// Define the SSID and password for the access point
const char* ssid = "ESP32-AP";
const char* password = "123456789";

// DHT sensor configuration
#define DHTPIN 4 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

float humidity;
float temperature;

// Create the WebServer object on port 80
WebServer server(80);

// HTML content to serve
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Monitor de Temperatura e Humidade</title>
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css">
  <style>
    body {
      font-family: Arial, sans-serif;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      margin: 0;
      background-color: #f7f7f7;
    }
    .container {
      text-align: center;
      background: #fff;
      padding: 20px;
      border-radius: 10px;
      box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
    }
    h1 {
      margin-bottom: 20px;
    }
    .data {
      font-size: 2rem;
    }
    .data p {
      margin: 10px 0;
    }
    .data i {
      color: #ca3517; /* Color for temperature icon */
    }
    .data i.fa-tint {
      color: #00add6; /* Color for humidity icon */
    }
    sup {
      font-size: 1.2rem;
    }
    .recommendations {
      margin-top: 20px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Monitor de Temperatura e Humidade</h1>
    <div class="data">
      <p>
        <i class="fas fa-thermometer-half"></i>
        <span>Temperatura: </span>
        <span id="temperature">--</span>
        <sup>&deg;C</sup>
      </p>
      <p>
        <i class="fas fa-tint"></i>
        <span>Humidade: </span>
        <span id="humidity">--</span>
        <sup>%</sup>
      </p>
    </div>
    <div class="recommendations">
      <h2>Recomendações</h2>
      <div id="recommendation">
        <!-- Recommendations will be displayed here -->
      </div>
    </div>
  </div>
  <script>
    document.addEventListener('DOMContentLoaded', function() {
      const temperatureElement = document.getElementById('temperature');
      const humidityElement = document.getElementById('humidity');
      const recommendationElement = document.getElementById('recommendation');
    
      function fetchData() {
        fetch('/data')
          .then(response => response.json())
          .then(data => {
            temperatureElement.textContent = data.temperature;
            humidityElement.textContent = data.humidity;
            displayRecommendations(data.temperature, data.humidity);
          })
          .catch(error => console.error('Erro ao buscar dados:', error));
      }
    
      function displayRecommendations(temperature, humidity) {
        let recommendations = [];
    
        if (temperature > 30) {
          recommendations.push('A temperatura está muito alta. Certifique-se de se manter hidratado.');
        } else if (temperature < 10) {
          recommendations.push('A temperatura está muito baixa. Mantenha-se aquecido.');
        } else {
          recommendations.push('A temperatura está agradável.');
        }
    
        if (humidity > 70) {
          recommendations.push('A umidade está muito alta. Pode causar desconforto.');
        } else if (humidity < 30) {
          recommendations.push('A umidade está muito baixa. Pode causar secura na pele.');
        } else {
          recommendations.push('A umidade está em um nível confortável.');
        }
    
        recommendationElement.innerHTML = recommendations.map(rec => `<p>${rec}</p>`).join('');
      }
    
      // Fetch data every 2 seconds
      setInterval(fetchData, 2000);
    
      // Fetch data immediately on page load
      fetchData();
    });
  </script>
</body>
</html>
)rawliteral";

// Function to serve the HTML page
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// Function to serve the sensor data as JSON
void handleData() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // Check if readings failed and try again
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha ao ler do sensor DHT!");
    humidity = 0.0;
    temperature = 0.0;
  }

  String json = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
  server.send(200, "application/json", json);
}

void setup() {
  // Start serial communication
  Serial.begin(115200);
  
  // Initialize the DHT sensor
  dht.begin();
  
  // Initialize the access point
  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Configure the server to serve the root page and sensor data
  server.on("/", handleRoot);
  server.on("/data", handleData);

  // Start the server
  server.begin();
}

void loop() {
  // Handle client requests
  server.handleClient();
}
