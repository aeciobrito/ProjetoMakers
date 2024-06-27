#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// Defina o SSID e a senha da sua rede Wi-Fi pessoal
const char* ssid = "plattinahs";
const char* password = "aeciobrito123";

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

  String html = "<!DOCTYPE html><html><head><title>ESP32 DHT22</title>\
  <meta http-equiv='refresh' content='10'/></head><body>";
  html += "<h1>ESP32 DHT22 Sensor</h1>";
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
  
  // Conectar-se à rede Wi-Fi pessoal
  WiFi.begin(ssid, password);
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  // Aguarde até que o ESP32 esteja conectado à rede Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Conectado à rede Wi-Fi!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

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
