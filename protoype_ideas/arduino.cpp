#include <WiFi.h>
#include <DHT.h>

// Define o pino do relé
const int pinoRele = 2;

// Define o sensor de temperatura
DHT dht(DHT11, 4);

// Define o endereço IP do servidor web
IPAddress ip(192, 168, 1, 1);

// Define a porta do servidor web
int porta = 80;

void setup() {
  // Configura o pino do relé como saída
  pinMode(pinoRele, OUTPUT);

  // Inicializa a Wi-Fi
  WiFi.begin("SSID", "SENHA");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Inicializa o servidor web
  WiFi.softAP(ip, porta);
}

void loop() {
  // Lê a temperatura atual
  float temperaturaAtual = dht.readTemperature();

  // Verifica se há uma solicitação HTTP
  WiFiClient client = server.available();
  if (client) {
    // Lê a solicitação HTTP
    String requisicao = client.readStringUntil('\n');

    // Verifica se a solicitação é para ligar o relé
    if (requisicao == "GET /ligar HTTP/1.1") {
      // Liga o relé
      digitalWrite(pinoRele, HIGH);
    }

    // Verifica se a solicitação é para desligar o relé
    if (requisicao == "GET /desligar HTTP/1.1") {
      // Desliga o relé
      digitalWrite(pinoRele, LOW);
    }

    // Envia uma resposta HTTP
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println();
    client.println("Rele ligado: " + String(digitalRead(pinoRele)));
    client.println();

    // Fecha a conexão com o cliente
    client.stop();
  }

  // Aguarda 1 segundo
  delay(1000);
}
