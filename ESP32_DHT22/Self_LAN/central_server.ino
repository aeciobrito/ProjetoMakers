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
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link
        href="https://fonts.googleapis.com/css2?family=Roboto:ital,wght@0,100;0,300;0,400;0,500;0,700;0,900;1,100;1,300;1,400;1,500;1,700;1,900&display=swap"
        rel="stylesheet">
    <script type="module" src="https://unpkg.com/ionicons@7.1.0/dist/ionicons/ionicons.esm.js"></script>
    <script nomodule src="https://unpkg.com/ionicons@7.1.0/dist/ionicons/ionicons.js"></script>
    <style>
    * {
    padding: 0;
    margin: 0;
    box-sizing: border-box;
    font-family: "Roboto", sans-serif;
  }
  
  ::-webkit-scrollbar {
    display: none;
  }
  
  .conteudo-home {
    display: flex;
    justify-content: center;
    align-items: center;
  }
  
  header {
    width: 100%;
    border-radius: 20px;
    padding-bottom: 5px;
    box-shadow:
      0 1px 1px rgba(0, 0, 0, 0.1),
      0 2px 2px rgba(0, 0, 0, 0.15),
      0 4px 4px rgba(0, 0, 0, 0.06),
      0 8px 8px rgba(0, 0, 0, 0.04),
      0 16px 16px rgba(0, 0, 0, 0.02);
  }
  
  .container-header {
    margin: 15px 30px 30px 30px;
  }
  
  .img-iconAlerta {
    display: flex;
    justify-content: space-between;
    align-items: center;
    width: 100%;
  
  
    & img {
      width: 80px;
      height: 80px;
      object-fit: cover;
      border-radius: 50%;
    }
  
    & ion-icon {
      font-size: 40px;
      color: #004A8D;
    }
  }
  
  .user {
    font-weight: 900;
  }
  
  .boasVindas {
    margin-left: 5px;
    margin-top: 15px;
    color: #263375;
    font-weight: 700;
    font-size: 1.1em;
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
    margin: 10px 10px;
    display: flex;
    justify-content: center;
    flex-direction: column;
    align-items: center;
    width: 90%;
    margin-bottom: 20px;
  
  }
  
  .cards {
    /* max-width: 280px; */
    width: 90%;
    max-height: 200px;
  }
  
  .cards a {
    text-decoration: none;
  }
  
  .cards img {
    /* max-width: 300px; */
    width: 100%;
    height: 170px;
    border-radius: 20px;
    object-fit: cover;
    filter: brightness(0.5);
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2), 0 4px 8px rgba(0, 0, 0, 0.15), 0 8px 16px rgba(0, 0, 0, 0.1), 0 12px 24px rgba(0, 0, 0, 0.05), 0 16px 32px rgba(0, 0, 0, 0.02);
  }
  
  .descricao1 {
    color: #FFF;
    position: relative;
    bottom: 58px;
    left: 16px;
    box-sizing: content-box;
    /* max-width: fit-content; */
    display: flex;
    /* justify-content: space-between; */
  
    & p {
      font-size: 0.7em;
    }
  }
  
  #weather {
    position: relative;
    bottom: 104px;
    right: 24px;
    font-size: 1em;
    font-weight: 600;
    display: flex;
    justify-content: center;
    flex-direction: column;
  
  }
  #temp-api{
    font-size: 1.4em;
    /* position: relative; */
    /* left: 30px; */
  }
  #min-max{
    font-size:0.8em;
    font-weight: 300;
    display: flex;
    gap: 5px;
  }
  
  #info-desc{
    font-size: 0.7em;
    
  }
  nav {
    position: fixed;
    bottom: 0;
    left: 0;
    width: 100%;
    height: 50px;
    border-radius: 20px;
    background-color: white;
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
  
  .ligar {
    display: flex;
    justify-content: center;
    align-items: center;
    background-color: white;
    border-radius: 50%;
    width: 90px;
    height: 90px;
    /* box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); */
    margin: 0 10px;
  }
  
  .ligar .power {
    font-size: 35px;
    color: #0a3d7d;
    position: relative;
    bottom: 10px;
  }
  </style>
</head>

<body onload="saudacao()">
    <header>
        <div class="container-header">
            <div class="img-iconAlerta">
                <img src="https://avatars.githubusercontent.com/u/153244771?v=4" alt="imagem de perfil">
                <ion-icon name="notifications-outline" class="notificao"></ion-icon>
            </div>
            <div class="boasVindas">
                <h2>Olá, <b class="user">Arthur</b></h2>
                <span id="saudacao"></span>
            </div>
        </div>
    </header>
    <main class="conteudo-home">
        <section class="container-home">
            <h1 class="t1">IOT Alarmes Inteligentes</h1>            
            %DATA%
        </section>
    </main>
  <script>
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

  function saudacao() {
    var data = new Date();
    var hora = data.getHours();
    var saudacao;

    if (hora >= 5 && hora < 13) {
        saudacao = 'Bom dia';
    } else if (hora >= 13 && hora < 18) {
        saudacao = 'Boa tarde';
    } else {
        saudacao = 'Boa noite';
    }

    document.getElementById('saudacao').innerText = saudacao + ", Bem Vindo!";
  }

  async function getWeather(city, apiKey) {
      const url = `https://api.openweathermap.org/data/2.5/weather?q=${city}&appid=${apiKey}&units=metric&lang=pt`;

      try {
          const response = await fetch(url);
          if (!response.ok) {
              throw new Error(`Erro ao fazer a solicitação: ${response.status}`);
          }
          const data = await response.json();
          const temp = data.main.temp;
          const tempMin = data.main.temp_min;
          const tempMax = data.main.temp_max;
          const humidity = data.main.humidity;
          const weatherDescription = data.weather[0].description;
          const weatherId = data.weather[0].id;

          const weatherIconClass = getWeatherIconClass(weatherId);

          document.getElementById('weather').innerHTML = `
              <div><span id="temp-api">${temp}° <i class="wi ${weatherIconClass}" id="icone-temperatura"></i><br></span>
              <p id="min-max">
              <i class="wi wi-direction-down">Min:${tempMin}°</i>
              <i class="wi wi-direction-up">Max:${tempMax}°</i><br>
              </p>
              </div>
            
          `;
          document.getElementById('info-desc').innerHTML = `
              <strong>Clima:</strong> ${weatherDescription.charAt(0).toUpperCase() + weatherDescription.slice(1)} <strong>Umidade:</strong> ${humidity}%
          `;
      } catch (error) {
          document.getElementById('weather').innerText = error.message;
          document.getElementById('info-desc').innerText = '';
      }
  }

  function getWeatherIconClass(weatherId) {
      if (weatherId >= 200 && weatherId < 300) {
          return 'wi-thunderstorm';
      } else if (weatherId >= 300 && weatherId < 400) {
          return 'wi-sprinkle';
      } else if (weatherId >= 500 && weatherId < 600) {
          return 'wi-rain';
      } else if (weatherId >= 600 && weatherId < 700) {
          return 'wi-snow';
      } else if (weatherId >= 700 && weatherId < 800) {
          return 'wi-fog';
      } else if (weatherId === 800) {
          return 'wi-day-sunny';
      } else if (weatherId > 800) {
          return 'wi-cloudy';
      } else {
          return 'wi-na';
      }
  }

  const apiKey = '94753d611e61a7b1aaadf87ad8a8b030';
  const city = 'São Paulo';

  // Função para atualizar o clima periodicamente
  function startWeatherUpdates() {
      getWeather(city, apiKey);
      setInterval(() => getWeather(city, apiKey), 300000); // Atualiza a cada 5 minutos
  }

  // Inicia as atualizações de clima
  startWeatherUpdates();
  </script>
</body>
</html>
)rawliteral";

// Function to handle root page
void handleRoot() {
  String dataHtml = "";
  for (int i = 0; i < roomCount; i++) {
    dataHtml += "<figure class='cards'><a href=#><img src='" + sensorDataArray[i].imageUrl + "' alt='Imagem do Local'><figcaption class='descricao1'> ";
    dataHtml += "<div><h4>" + sensorDataArray[i].location + "</h4><p>" + String(sensorDataArray[i].temperature) + " &deg;C, " + String(sensorDataArray[i].humidity) + " %</p></div>";
    dataHtml += "<div><h4>Temperatura Externa</h4><p id='info-desc'>...</p></div><div class='recommendations'><h2>Recomendações</h2><div id='recommendation'></div></div><div id='weather'></div></figcaption></a></figure>";
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
    sensorDataArray[roomCount].location = location;
    sensorDataArray[roomCount].temperature = temperature;
    sensorDataArray[roomCount].humidity = humidity;
    sensorDataArray[roomCount].imageUrl = imageUrl;
    roomCount++;
  }

  server.send(200, "text/plain", "Data received");
}

void setup() {
  // Start serial communication
  Serial.begin(115200);

// Connect to router for internet access
  WiFi.begin(routerSsid, routerPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting to WiFi...");
    Serial.println(routerSsid);
  }
  Serial.print("Connected to WiFi: ");
  Serial.println(routerSsid);

  // Start the access point
  WiFi.softAP(ssid, password);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Localizar pela url esp32.local ao invés do IP
  if(MDNS.begin("esp32")) {
    Serial.println("MDNS responder inicializado");
  }

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