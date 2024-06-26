#include "DHT.h"

// Defina o pino do sensor e o tipo de sensor
#define DHTPIN 4
#define DHTTYPE DHT22

// Inicialize o objeto DHT
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  // Aguarde alguns segundos entre as leituras
  delay(2000);

  // Leitura da temperatura e da umidade
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

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
