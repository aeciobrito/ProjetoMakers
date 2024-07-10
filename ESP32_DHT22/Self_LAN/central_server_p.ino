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
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Monitor de Temperatura e Humidade</title>
  <link rel="stylesheet" href="https://fonts.googleapis.com/css2?family=Roboto:wght@100;300;400;500;700;900&display=swap">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/weather-icons/2.0.9/css/weather-icons.min.css">
  <style>
    * { padding: 0; margin: 0; box-sizing: border-box; font-family: 'Roboto', sans-serif; }
    ::-webkit-scrollbar { display: none; }
    .conteudo-home { display: flex; justify-content: center; align-items: center; }
    header { width: 100%; border-radius: 20px; padding-bottom: 5px; box-shadow: 0 1px 1px rgba(0, 0, 0, 0.1), 0 2px 2px rgba(0, 0, 0, 0.15), 0 4px 4px rgba(0, 0, 0, 0.06), 0 8px 8px rgba(0, 0, 0, 0.04), 0 16px 16px rgba(0, 0, 0, 0.02); }
    .container-header { margin: 15px 30px 30px 30px; }
    .img-iconAlerta { display: flex; justify-content: space-between; align-items: center; width: 100%; }
    .img-iconAlerta img { width: 80px; height: 80px; object-fit: cover; border-radius: 50%; }
    .img-iconAlerta ion-icon { font-size: 40px; color: #004A8D; }
    .user { font-weight: 900; }
    .boasVindas { margin-left: 5px; margin-top: 15px; color: #263375; font-weight: 700; font-size: 1.1em; }
    .boasVindas h2 { font-weight: 300; }
    .t1 { text-align: center; font-size: 1.1em; font-weight: 700; margin: 20px; }
    .container-home { margin: 10px 10px; display: flex; justify-content: center; flex-direction: column; align-items: center; width: 90%; margin-bottom: 20px; }
    .cards { width: 90%; max-height: 200px; }
    .cards a { text-decoration: none; }
    .cards img { width: 100%; height: 170px; border-radius: 20px; object-fit: cover; filter: brightness(0.5); box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2), 0 4px 8px rgba(0, 0, 0, 0.15), 0 8px 16px rgba(0, 0, 0, 0.1), 0 12px 24px rgba(0, 0, 0, 0.05), 0 16px 32px rgba(0, 0, 0, 0.02); }
    .descricao1 { color: #FFF; position: relative; bottom: 58px; left: 16px; }
    .descricao1 p { font-size: 0.7em; }
    #weather { position: relative; bottom: 104px; right: 24px; font-size: 1em; font-weight: 600; display: flex; justify-content: center; flex-direction: column; }
    #temp-api { font-size: 1.4em; }
    #min-max { font-size: 0.8em; font-weight: 300; display: flex; gap: 5px; }
    #info-desc { font-size: 0.7em; }
    nav { position: fixed; bottom: 0; left: 0; width: 100%; height: 50px; border-radius: 20px; background-color: white; box-shadow: 0 -4px 6px rgba(0, 0, 0, 0.1); display: flex; justify-content: center; align-items: center; z-index: 1000; }
    .itens-nav { display: flex; justify-content: space-between; align-items: center; width: 200px; }
    .itens-nav ion-icon { font-size: 24px; color: #0a3d7d; cursor: pointer; }
    .ligar { display: flex; justify-content: center; align-items: center; background-color: white; border-radius: 50%; width: 90px; height: 90px; margin: 0 10px; }
    .ligar .power { font-size: 35px; color: #0a3d7d; position: relative; bottom: 10px; }
  </style>
</head>
<body onload="saudacao()">
  <header>
    <div class="container-header">
      <div class="img-iconAlerta">
        <img src="https://www.pngall.com/wp-content/uploads/5/User-Profile-PNG-Free-Download.png" alt="imagem de perfil">
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
        if (!response.ok) throw new Error('Error fetching weather data');
        const data = await response.json();

        const temperature = data.main.temp.toFixed(0);
        const minTemp = data.main.temp_min.toFixed(0);
        const maxTemp = data.main.temp_max.toFixed(0);
        const description = data.weather[0].description;
        const iconCode = data.weather[0].icon;
        const iconClass = getWeatherIconClass(iconCode);

        document.getElementById('temp-api').innerText = `${temperature}°C`;
        document.getElementById('min-max').innerText = `Min: ${minTemp}°C | Máx: ${maxTemp}°C`;
        document.getElementById('info-desc').innerText = description;
        document.getElementById('weather-icon').className = iconClass;
      } catch (error) {
        console.error('Error fetching weather data:', error);
      }
    }

    function getWeatherIconClass(iconCode) {
      const iconMap = {
        '01d': 'wi-day-sunny',
        '01n': 'wi-night-clear',
        '02d': 'wi-day-cloudy',
        '02n': 'wi-night-alt-cloudy',
        '03d': 'wi-cloud',
        '03n': 'wi-cloud',
        '04d': 'wi-cloudy',
        '04n': 'wi-cloudy',
        '09d': 'wi-day-showers',
        '09n': 'wi-night-alt-showers',
        '10d': 'wi-day-rain',
        '10n': 'wi-night-alt-rain',
        '11d': 'wi-day-thunderstorm',
        '11n': 'wi-night-alt-thunderstorm',
        '13d': 'wi-day-snow',
        '13n': 'wi-night-alt-snow',
        '50d': 'wi-day-fog',
        '50n': 'wi-night-fog'
      };
      return iconMap[iconCode] || 'wi-na';
    }
  </script>
</body>
</html>
)rawliteral";

// Function to handle root URL "/"
void handleRoot() {
  String html = htmlPage;
  String data = "";
  for (int i = 0; i < roomCount; i++) {
    data += "<div class='cards'>";
    data += "<a href='#'>";
    data += "<img src='https://www.example.com/room_image.jpg' alt='Room Image'>";
    data += "<div class='descricao1'>";
    data += "<p>Local: " + sensorDataArray[i].location + "</p>";
    data += "<p>Temperatura: " + String(sensorDataArray[i].temperature) + "°C</p>";
    data += "<p>Humidade: " + String(sensorDataArray[i].humidity) + "%</p>";
    data += "</div>";
    data += "<div id='weather'>";
    data += "<i id='weather-icon' class='wi wi-day-sunny'></i>";
    data += "<span id='temp-api'>--°C</span>";
    data += "<span id='min-max'>Min: --°C | Máx: --°C</span>";
    data += "<span id='info-desc'>Descrição do clima</span>";
    data += "</div>";
    data += "</a>";
    data += "</div>";
  }
  html.replace("%DATA%", data);
  server.send(200, "text/html", html);
}

// Function to handle data submission
void handleSubmit() {
  if (server.hasArg("location") && server.hasArg("temperature") && server.hasArg("humidity")) {
    SensorData data;
    data.location = server.arg("location");
    data.temperature = server.arg("temperature").toFloat();
    data.humidity = server.arg("humidity").toFloat();

    if (roomCount < maxRooms) {
      sensorDataArray[roomCount++] = data;
    }

    server.send(200, "text/plain", "Data received");
  } else {
    server.send(400, "text/plain", "Missing data");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.println("Access point started");
  Serial.println("IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/submit", HTTP_POST, handleSubmit);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
