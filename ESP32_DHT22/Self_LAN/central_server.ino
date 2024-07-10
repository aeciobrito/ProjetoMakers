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
  <title>Ambiente Controlado</title>
  <link rel='preconnect' href='https://fonts.googleapis.com'>
  <link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>
  <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css'>
  <link href='https://fonts.googleapis.com/css2?family=Roboto:wght@100;300;400;500;700;900&display=swap' rel='stylesheet'>
  <script type='module' src='https://unpkg.com/ionicons@7.1.0/dist/ionicons/ionicons.esm.js'></script>
  <script nomodule src='https://unpkg.com/ionicons@7.1.0/dist/ionicons/ionicons.js'></script>
  <style>
    * { padding: 0; margin: 0; box-sizing: border-box; font-family: 'Roboto', sans-serif; }
    body { background-image: linear-gradient(rgba(0, 0, 0, 0) 20%, rgba(255, 255, 255, 1) 80%), url(assets/greenhouse2.jpg); background-size: cover; background-repeat: no-repeat; height: 100vh; background-position: center center; }
    header { background-color: white; border-radius: 0 0 0 20px; padding: 15px 10px; width: 100%; border-radius: 20px; padding-bottom: 5px; box-shadow: 0 1px 1px rgba(0, 0, 0, 0.1), 0 2px 2px rgba(0, 0, 0, 0.15), 0 4px 4px rgba(0, 0, 0, 0.06), 0 8px 8px rgba(0, 0, 0, 0.04), 0 16px 16px rgba(0, 0, 0, 0.02); }
    .container-header { margin: 15px 30px 30px 30px; }
    .img-iconAlerta { display: flex; justify-content: space-between; align-items: center; width: 100%; }
    .img-iconAlerta img { width: 80px; height: 80px; object-fit: cover; border-radius: 50%; }
    .img-iconAlerta ion-icon { font-size: 40px; color: #004A8D; }
    .user { font-weight: 900; }
    .boasVindas { margin-left: 5px; margin-top: 15px; color: #263375; font-weight: 700; font-size: 1.1em; }
    .boasVindas h2 { font-weight: 300; }
    main { display: grid; grid-template-columns: 1fr; }
    .itens-nav { width: 70%; max-width: 400px; }
    .itens-nav { display: flex; justify-content: space-between; align-items: center; }
    .itens-nav ion-icon { font-size: 24px; color: #0a3d7d; cursor: pointer; }
    .ligar { display: flex; justify-content: center; align-items: center; background-color: white; border-radius: 50%; width: 90px; height: 90px; margin: 0 10px; }
    .ligar .power { font-size: 35px; color: #0a3d7d; position: relative; bottom: 10px; }
    .status_control { margin: 30px 20px; padding: 20px 10%; background-color: #263375; color: white; font-size: 1.2em; border-radius: 20px; text-align: center; display: flex; flex-direction: column; gap: 15px; }
    .temperature-container { display: grid; grid-template-columns: 2fr 1fr; margin: 10px; padding: 10px; gap: 10px; justify-items: center; align-items: center; }
    .current-temperature { border-radius: 50%; width: 55vw; height: 55vw; color: white; font-size: 10vw; text-align: center; display: flex; justify-content: center; align-items: center; background-color: #ffffff; box-shadow: 0px 0px 15px 5px #263375; color: #263375; }
    .other_temperatures { display: grid; gap: 10px; justify-items: center; }
    .outside-temperature { background-color: #263375; height: 28vw; width: 28vw; border-radius: 30%; color: white; display: flex; gap: 10px; flex-direction: column; justify-content: center; align-items: center; }
    .subtitle-other-temperature { font-size: 4vw; }
    .title-other-temperature { font-size: 5vw; }
    .alarm-container { display: grid; grid-template-columns: 1fr 1fr; justify-items: center; margin: 20px 0; }
    .alarm-control { background-color: #263375; height: 35vw; width: 40vw; border-radius: 20px; color: white; display: flex; flex-direction: column; justify-content: center; align-items: center; gap: 15px; padding: 0 15px; }
    .subtitle-alarm-control { font-size: 4vw; padding: 0 5px; }
    .title-alarm-control { font-size: 7vw; }
    nav { position: fixed; bottom: 0; left: 0; width: 100%; height: 50px; border-radius: 20px; background-color: white; box-shadow: 0 -4px 6px rgba(0, 0, 0, 0.1); display: flex; justify-content: center; align-items: center; z-index: 1000; }
    .humidity-icon { display: inline-block; vertical-align: middle; margin-right: 5px; }
    .humidity-text { display: inline-block; vertical-align: middle; }
  </style>
</head>
<body onload='startWeatherUpdates()'>
  <header>
    <div class='container-header'>
      <div class='img-iconAlerta'>
        <img src='https://avatars.githubusercontent.com/u/153244771?v=4' alt='imagem de perfil'>
        <ion-icon name='notifications-outline' class='notificao'></ion-icon>
      </div>
      <div class='boasVindas'>
        <h2>Olá, <b class='user'>Arthur</b></h2>
        <span id='saudacao'></span>
      </div>
    </div>
  </header>
  <main>
    <div class='status_control' id='statusControl'>
      <h2 id='temp-status'>Status atual</h2>
      <h4 id='humi-status'>Umidade atual</h4>
    </div>
    <section class='temperature-container'>
      <div class='current-temperature'>
        <div id='temperature'></div>°C
      </div>
      <div class='other_temperatures'>
        <div class='outside-temperature warning'>
          <span class='subtitle-other-temperature'>Humidade</span>
          <h3 class='title-other-temperature'>
            <i class='fas fa-tint humidity-icon' style='color: rgba(85, 158, 254, 1);'></i>
            <span id='humidity' class='humidity-text'></span>
          </h3>
        </div>
        <div class='outside-temperature warning'>
          <span class='subtitle-other-temperature'>Externa</span>
          <h3 class='title-other-temperature' id='extTemp'>
            <i class='icone1 fas fa-temperature-low' style='color: rgba(255, 70, 70, 1);'></i> --°C
          </h3>
        </div>
      </div>
    </section>
    <div class='alarm-container'>
      <div class='alarm-control' id='minimumTemp'>
        <span class='subtitle-alarm-control'>Temp. Mínima</span>
        <h3 class='title-alarm-control'>10°C</h3>
      </div>
      <div class='alarm-control' id='maximumTemp'>
        <span class='subtitle-alarm-control'>Temp. Máxima</span>
        <h3 class='title-alarm-control'>25°C</h3>
      </div>
    </div>
  </main>
  <nav>
    <div class='itens-nav'>
      <ion-icon name='home-outline'></ion-icon>
      <ion-icon name='bar-chart-outline'></ion-icon>
      <div class='ligar'>
        <ion-icon name='power-outline' class='power'></ion-icon>
      </div>
      <ion-icon name='settings-outline'></ion-icon>
      <ion-icon name='people-outline'></ion-icon>
    </div>
  </nav>
  <script>
    function startWeatherUpdates() {
      setInterval(getData, 2000);
    }

    function getData() {
      fetch('/sensorData')
        .then(response => response.json())
        .then(data => updateUI(data));
    }

    function updateUI(data) {
      const tempStatus = document.getElementById('temp-status');
      const humiStatus = document.getElementById('humi-status');
      const temperatureDiv = document.getElementById('temperature');
      const humidityDiv = document.getElementById('humidity');

      // Update the first sensor data
      if (data.length > 0) {
        const sensorData = data[0];
        tempStatus.textContent = `Temperatura ${sensorData.location}`;
        humiStatus.textContent = `Umidade ${sensorData.location}`;
        temperatureDiv.textContent = sensorData.temperature.toFixed(1);
        humidityDiv.textContent = `${sensorData.humidity.toFixed(1)}%`;
      }
    }
  </script>
</body>
</html>
)rawliteral";

// Function to return sensor data as JSON
void handleSensorData() {
  StaticJsonDocument<2048> jsonDocument;  // Increased size for more data

  JsonArray jsonArray = jsonDocument.to<JsonArray>();

  for (int i = 0; i < roomCount; i++) {
    JsonObject roomData = jsonArray.createNestedObject();
    roomData["location"] = sensorDataArray[i].location;
    roomData["temperature"] = sensorDataArray[i].temperature;
    roomData["humidity"] = sensorDataArray[i].humidity;
  }

  String jsonString;
  serializeJson(jsonDocument, jsonString);
  server.send(200, "application/json", jsonString);
}

// Function to handle root endpoint
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// Function to set up the server and WiFi
void setup() {
  Serial.begin(115200);

  // Initialize sensor data array
  for (int i = 0; i < maxRooms; i++) {
    sensorDataArray[i] = {"Room " + String(i + 1), 0.0, 0.0};
  }
  roomCount = maxRooms;

  // Start WiFi access point
  WiFi.softAP(ssid, password);
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Set up URL handlers
  server.on("/", handleRoot);
  server.on("/sensorData", handleSensorData);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Simulate sensor data for demonstration purposes
  for (int i = 0; i < roomCount; i++) {
    sensorDataArray[i].temperature = 20.0 + (rand() % 100) / 10.0; // Random temperature
    sensorDataArray[i].humidity = 50.0 + (rand() % 100) / 10.0;    // Random humidity
  }

  server.handleClient();
}
