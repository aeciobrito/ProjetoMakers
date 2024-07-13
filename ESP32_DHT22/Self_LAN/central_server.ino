#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>

// Access point credentials
const char* ssid = "ESP32-AP";
const char* password = "123456789";

// Router credentials for internet access
const char* routerSsid = "plattina";
const char* routerPassword = "aeciobrito123";

// Create the WebServer object on port 80
WebServer server(80);

// Data structure to hold sensor data
struct SensorData {
  String location;
  float temperature;
  float humidity;
  String imageUrl;
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
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Monitor de Temperatura e Humidade</title>
  <style>
    * {
      padding: 0;
      margin: 0;
      box-sizing: border-box;
      font-family: "Roboto", sans-serif;
    }
    body {
      background-color: #f0f0f0;
    }
    header {
      width: 100%;
      border-radius: 20px;
      padding-bottom: 5px;
      box-shadow: 0 1px 1px rgba(0, 0, 0, 0.1), 0 2px 2px rgba(0, 0, 0, 0.15), 0 4px 4px rgba(0, 0, 0, 0.06), 0 8px 8px rgba(0, 0, 0, 0.04), 0 16px 16px rgba(0, 0, 0, 0.02);
      background-color: #ffffff;
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 10px 30px;
    }
    .container-header {
      display: flex;
      align-items: center;
      width: 100%;
      justify-content: space-between;
    }
    .img-iconAlerta img {
      width: 80px;
      height: 80px;
      object-fit: cover;
      border-radius: 50%;
    }
    .img-iconAlerta ion-icon {
      font-size: 40px;
      color: #004a8d;
    }
    .boasVindas {
      margin-left: 10px;
      color: #263375;
      font-weight: 700;
      font-size: 1.1em;
      display: flex;
      flex-direction: column;
      align-items: flex-start;
    }
    .boasVindas h2 {
      font-weight: 300;
    }
    .t1 {
      text-align: center;
      font-size: 1.1em;
      font-weight: 700;
      margin: 20px;
    }
    .container-home {
      margin: 10px;
      display: flex;
      flex-wrap: wrap;
      justify-content: center;
      gap: 20px;
    }
    .cards {
      width: 90%;
      max-height: 200px;
      background-color: #ffffff;
      border-radius: 20px;
      overflow: hidden;
      box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2), 0 4px 8px rgba(0, 0, 0, 0.15), 0 8px 16px rgba(0, 0, 0, 0.1), 0 12px 24px rgba(0, 0, 0, 0.05), 0 16px 32px rgba(0, 0, 0, 0.02);
    }
    .cards a {
      text-decoration: none;
      color: inherit;
    }
    .cards img {
      width: 100%;
      height: 100%;
      object-fit: cover;
      filter: brightness(0.5);
      border-radius: 20px 20px 0 0;
    }
    .descricao1 {
      position: relative;
      bottom: 58px;
      left: 16px;
      color: #fff;
    }
    .descricao1 p {
      font-size: 1.3em;
    }
    nav {
      position: fixed;
      bottom: 0;
      left: 0;
      width: 100%;
      height: 50px;
      border-radius: 20px 20px 0 0;
      background-color: #ffffff;
      box-shadow: 0 -4px 6px rgba(0, 0, 0, 0.1);
      display: flex;
      justify-content: center;
      align-items: center;
      z-index: 1000;
    }
    .itens-nav {
      display: flex;
      justify-content: space-between;
      align-items: center;
      width: 200px;
    }
    .itens-nav ion-icon {
      font-size: 24px;
      color: #0a3d7d;
      cursor: pointer;
    }
    .weather-container {
      text-align: right;
      color: #263375;
      display: flex;
      flex-direction: column;
      align-items: flex-end;
    }
    .weather-container #temp-api {
      font-size: 1.4em;
      font-weight: 700;
    }
    .weather-container #min-max {
      font-size: 0.8em;
      font-weight: 300;
      display: flex;
      gap: 5px;
      justify-content: flex-end;
    }
    .weather-container #info-desc {
      font-size: 0.7em;
    }
  </style>
</head>
<body>
  <header>
    <div class="container-header">
      <div class="img-iconAlerta">
        <img src="https://brasil.campus-party.org/wp-content/uploads/2024/03/cpbr16_elements__7.png" alt="imagem de perfil">
        <ion-icon name="notifications-outline" class="notificao"></ion-icon>
      </div>
      <div class="boasVindas">
        <h2>Olá, <b class="user">Campuseiro</b></h2>
      </div>
      <div class="weather-container" id="weather">
        <div id="temp-api"></div>
        <div id="min-max"></div>
        <div id="info-desc"></div>
      </div>
    </div>
  </header>
  </header>
  <main class="conteudo-home">
    <section class="container-home">
      <h1 class="t1">IOT Alarmes Inteligentes</h1>            
      %DATA%
    </section>
  </main>  
  <script>
  window.onload = function() {
      fetchDataAndUpdate();
      setInterval(fetchDataAndUpdate, 500);
    }

    function fetchDataAndUpdate() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          const containerHome = document.querySelector('.container-home');
          containerHome.innerHTML = '';
          data.forEach(room => {
            const card = document.createElement('figure');
            card.classList.add('cards');
            card.innerHTML = `
              <a href="#">
                <img src="${room.imageUrl}" alt="Imagem do Local">
                <figcaption class="descricao1">
                  <div><h4>${room.location}</h4><p>${room.temperature.toFixed(0)} &deg;C, ${room.humidity.toFixed(0)} %</p></div>
                </figcaption>
              </a>
            `;
            containerHome.appendChild(card);
          });
        })
        .catch(error => console.error('Error fetching data:', error));
    }

    function startWeatherUpdates() {
      const apiKey = '94753d611e61a7b1aaadf87ad8a8b030';
      const city = 'São Paulo';
      getWeather(city, apiKey);      
    }

    async function getWeather(city, apiKey) {
      const url = `https://api.openweathermap.org/data/2.5/weather?q=${city}&appid=${apiKey}&units=metric&lang=pt`;
      try {
        const response = await fetch(url);
        if (!response.ok) {
          throw new Error(`Erro ao fazer a solicitação: ${response.status}`);
        }
        const data = await response.json();
        const tempElement = document.getElementById("temp-api");
        const minMaxElement = document.getElementById("min-max");
        const descElement = document.getElementById("info-desc");

        const temperature = data.main.temp.toFixed(0);
        const minTemperature = data.main.temp_min.toFixed(0);
        const maxTemperature = data.main.temp_max.toFixed(0);
        const description = data.weather[0].description;

        tempElement.innerText = `${temperature}°C`;
        minMaxElement.innerHTML = `<span>Min: ${minTemperature}°C</span><span>Max: ${maxTemperature}°C</span>`;
        descElement.innerText = description.charAt(0).toUpperCase() + description.slice(1);
      } catch (error) {
        console.error('Erro ao obter a previsão do tempo:', error);
      }
    }

    setInterval(startWeatherUpdates, 30000);
    startWeatherUpdates();
  </script>
</body>
</html>
)rawliteral";

// Handles the root path and serves the HTML page
void handleRoot() {
  String dataHtml;
  for (int i = 0; i < roomCount; i++) {
    dataHtml += "<figure class='cards'>";
    dataHtml += "<a href='#'>";
    dataHtml += "<img src='" + sensorDataArray[i].imageUrl + "' alt='Imagem do Local'>";
    dataHtml += "<figcaption class='descricao1'>";
    dataHtml += "<div><h4>" + sensorDataArray[i].location + "</h4><p>" + sensorDataArray[i].temperature + " &deg;C, " + sensorDataArray[i].humidity + " %</p></div>";
    dataHtml += "<div id='weather'><div id='temp-api'></div><div id='min-max'></div><div id='info-desc'></div></div>";
    dataHtml += "</figcaption></a></figure>";
  }
  
  String html = htmlPage;
  html.replace("%DATA%", dataHtml);
  html.replace("%TEMPERATURE%", roomCount > 0 ? String(sensorDataArray[0].temperature) : "0");
  html.replace("%HUMIDITY%", roomCount > 0 ? String(sensorDataArray[0].humidity) : "0");
  html.replace("%ROOMCOUNT%", String(roomCount));
  server.send(200, "text/html", html);
}

// Handles the data endpoint for sensor data
void handleData() {
  DynamicJsonDocument doc(2048);
  JsonArray data = doc.to<JsonArray>();
  for (int i = 0; i < roomCount; i++) {
    JsonObject room = data.createNestedObject();
    room["location"] = sensorDataArray[i].location;
    room["temperature"] = sensorDataArray[i].temperature;
    room["humidity"] = sensorDataArray[i].humidity;
    room["imageUrl"] = sensorDataArray[i].imageUrl;
  }
  String jsonData;
  serializeJson(doc, jsonData);
  server.send(200, "application/json", jsonData);
}

// Handles POST data from sensor nodes
void handlePostData() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Body not received");
    return;
  }

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }

  String location = doc["location"];
  float temperature = doc["temperature"];
  float humidity = doc["humidity"];
  String imageUrl = doc["imageUrl"];

  bool roomExists = false;
  for (int i = 0; i < roomCount; i++) {
    if (sensorDataArray[i].location == location) {
      sensorDataArray[i].temperature = temperature;
      sensorDataArray[i].humidity = humidity;
      sensorDataArray[i].imageUrl = imageUrl;
      roomExists = true;
      break;
    }
  }

  if (!roomExists && roomCount < maxRooms) {
    sensorDataArray[roomCount++] = {location, temperature, humidity, imageUrl};
  }

  server.send(200, "text/plain", "Data received");
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(routerSsid, routerPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting to WiFi...");
  }
  Serial.print("Connected to WiFi: ");
  Serial.println(WiFi.localIP());

  // Set up the access point
  WiFi.softAP(ssid, password);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Set up mDNS responder
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder inicializado");
  }

  // Define web server routes
  server.on("/", handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.on("/post-data", HTTP_POST, handlePostData);

  // Start the web server
  server.begin();
}

void loop() {
  server.handleClient();
}