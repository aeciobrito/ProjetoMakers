<?php
require 'config.php';

$esp32_url = 'http://esp32.local/data'; // URL do ESP32

// Obter dados do ESP32
$data = file_get_contents($esp32_url);
$json = json_decode($data, true);

$temperature = $json['temperature'];
$humidity = $json['humidity'];

// Inserir dados no banco de dados
$stmt = $pdo->prepare('INSERT INTO readings (temperature, humidity) VALUES (?, ?)');
$stmt->execute([$temperature, $humidity]);

echo 'Dados armazenados com sucesso!';
?>
