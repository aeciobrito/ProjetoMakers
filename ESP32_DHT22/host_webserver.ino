#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// Defina o SSID e a senha para o ponto de acesso
const char* ssid = "ESP32-AP";
const char* password = "123456789";

// Configuração do sensor DHT
#define DHTPIN 4 // Pino digital conectado ao sensor DHT
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

float humidity;
float temperature;

// Crie o objeto WebServer na porta 80
WebServer server(80);

// Função para servir a página HTML
void handleRoot() {
  // Leia os valores do sensor DHT
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // Verifica se as leituras falharam e tenta novamente
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha ao ler do sensor DHT!");
    humidity = 0.0;
    temperature = 0.0;
  }

  String html = "<!DOCTYPE html><html><head><title>ESP32 DHT11</title>\
  <meta http-equiv='refresh' content='2'/></head><body>";
  html += "<h1>ESP32 DHT11 Sensor</h1>";
  html += "<p>Temperature: " + String(temperature) + " &deg;C</p>";
  html += "<p>Humidity: " + String(humidity) + " %</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  // Iniciar comunicação serial
  Serial.begin(115200);
  
  // Inicializar o sensor DHT
  dht.begin();
  
  // Inicializar o ponto de acesso
  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Configurar o servidor para servir a página root
  server.on("/", handleRoot);

  // Iniciar o servidor
  server.begin();
}

void loop() {
  // Tratar as requisições do servidor
  server.handleClient();

  // Aguarde alguns segundos entre as leituras
  delay(2000);

  // Leia os valores do sensor DHT
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // Verifica se as leituras falharam e tenta novamente
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha ao ler do sensor DHT!");
    return;
  }

  // Exibe os valores no Serial Monitor
  Serial.print("Umidade: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" *C");
}
