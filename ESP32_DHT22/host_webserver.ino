#include <WiFi.h>
#include "DHT.h"

// Defina o pino do sensor e o tipo de sensor
#define DHTPIN 4
#define DHTTYPE DHT22

// Inicialize o objeto DHT
DHT dht(DHTPIN, DHTTYPE);

// Defina o SSID e a senha para o ponto de acesso
const char* ssid = "ESP32-AP";
const char* password = "123456789";

// Crie um objeto WiFiServer na porta 80
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Inicialize o ponto de acesso
  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Inicie o servidor
  server.begin();
}

void loop() {
  // Verifica se há algum cliente conectado
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Aguarde até que o cliente envie dados
  Serial.println("New Client.");
  while (!client.available()) {
    delay(1);
  }

  // Leia a solicitação do cliente
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Leia a temperatura e a umidade
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Verifique se as leituras falharam e tente novamente
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha ao ler do sensor DHT!");
    return;
  }

  // Crie a página web
  String s = "<html>\
  <head>\
    <meta http-equiv='refresh' content='4'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>ESP32 DHT Server</title>\
    <style>\
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
    h2 { font-size: 3.0rem; }\
    p { font-size: 3.0rem; }\
    .units { font-size: 1.2rem; }\
    .dht-labels{ font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}\
    </style>\
  </head>\
  <body>\
      <h2>ESP32 DHT Server!</h2>\
      <p>\
        <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
        <span class='dht-labels'>Temperature</span>\
        <span>%.2f</span>\
        <sup class='units'>&deg;C</sup>\
      </p>\
      <p>\
        <i class='fas fa-tint' style='color:#00add6;'></i>\
        <span class='dht-labels'>Humidity</span>\
        <span>%.2f</span>\
        <sup class='units'>&percnt;</sup>\
      </p>\
  </body>\
</html>";

  // Envie a resposta ao cliente
  client.print(s);
  delay(1);
  Serial.println("Client disconnected.");
}