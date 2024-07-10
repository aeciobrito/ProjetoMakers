#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// Access point credentials
const char* ssid = "ESP32-AP";
const char* password = "123456789";

// Create the WebServer object on port 80
WebServer server(80);

// Data structure to hold sensor data
struct SensorData {
  String location;
  float temperature;
  float humidity;
};

// Array to hold data from multiple rooms
const int maxRooms = 10;
SensorData sensorDataArray[maxRooms];
int roomCount = 0;

// HTML page template
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang='pt-br'>
<head>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>Monitor de Temperatura e Humidade</title>
  <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css'>
  <style>
    body { font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; background-color: #f7f7f7; }
    .container { text-align: center; background: #fff; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }
    h1 { margin-bottom: 20px; }
    .data { font-size: 2rem; }
    .data p { margin: 10px 0; }
    .data i { color: #ca3517; }
    .data i.fa-tint { color: #00add6; }
    .card { padding: 10px; margin: 10px; border: 1px solid #ccc; border-radius: 5px; cursor: pointer; }
    .hidden { display: none; }
  </style>
</head>
<body>
  <div class='container'>
    <h1>Monitor de Temperatura e Humidade</h1>
    <div class='data'>
      %DATA%
    </div>
    <div class='recommendations'>
      <h2>Recomendações</h2>
      <div id='recommendation'></div>
    </div>
  </div>
  <script>
    function showDetails(index) {
      var details = document.getElementById('details-' + index);
      if (details.classList.contains('hidden')) {
        details.classList.remove('hidden');
      } else {
        details.classList.add('hidden');
      }
    }
    document.addEventListener('DOMContentLoaded', function() {
      const recommendationElement = document.getElementById('recommendation');
      let recommendations = [];
      const rooms = %ROOMCOUNT%;
      const temperature = %TEMPERATURE%;
      const humidity = %HUMIDITY%;
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
    });
  </script>
</body>
</html>
)rawliteral";

// Function to handle root page
void handleRoot() {
  String dataHtml = "";
  for (int i = 0; i < roomCount; i++) {
    dataHtml += "<div class='card' onclick='showDetails(" + String(i) + ")'><i class='fas fa-thermometer-half'></i> <span>" + sensorDataArray[i].location + " - " + String(sensorDataArray[i].temperature) + " &deg;C, " + String(sensorDataArray[i].humidity) + " %</span></div>";
    dataHtml += "<div id='details-" + String(i) + "' class='hidden'><p>Temperatura: " + String(sensorDataArray[i].temperature) + " &deg;C</p><p>Humidade: " + String(sensorDataArray[i].humidity) + " %</p></div>";
  }
  
  String html = htmlPage;
  html.replace("%DATA%", dataHtml);
  html.replace("%ROOMCOUNT%", String(roomCount));
  if (roomCount > 0) {
    html.replace("%TEMPERATURE%", String(sensorDataArray[0].temperature));
    html.replace("%HUMIDITY%", String(sensorDataArray[0].humidity));
  } else {
    html.replace("%TEMPERATURE%", "0");
    html.replace("%HUMIDITY%", "0");
  }

  server.send(200, "text/html", html);
}

// Function to handle POST data
void handlePostData() {
  if (server.hasArg("plain") == false) { // Check if body is received
    server.send(400, "text/plain", "Body not received");
    return;
  }

  String body = server.arg("plain");
  Serial.println("Received data: " + body);

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  String location = doc["location"];
  float temperature = doc["temperature"];
  float humidity = doc["humidity"];

  bool roomExists = false;

  for (int i = 0; i < roomCount; i++) {
    if (sensorDataArray[i].location == location) {
      sensorDataArray[i].temperature = temperature;
      sensorDataArray[i].humidity = humidity;
      roomExists = true;
      break;
    }
  }

  if (!roomExists && roomCount < maxRooms) {
    sensorDataArray[roomCount].location = location;
    sensorDataArray[roomCount].temperature = temperature;
    sensorDataArray[roomCount].humidity = humidity;
    roomCount++;
  }

  server.send(200, "text/plain", "Data received");
}

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Start the access point
  WiFi.softAP(ssid, password);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Configure the server to serve the root page and handle POST data
  server.on("/", handleRoot);
  server.on("/post-data", HTTP_POST, handlePostData);

  // Start the server
  server.begin();
}

void loop() {
  // Handle client requests
  server.handleClient();
}
