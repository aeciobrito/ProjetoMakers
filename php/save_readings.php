<?php
require 'config.php';

$esp32_url = 'http://esp32.local/data'; // URL do ESP32

function saveData($pdo, $esp32_url) {
    // Obter dados do ESP32
    $data = file_get_contents($esp32_url);
    $json = json_decode($data, true);

    if (isset($json['temperature']) && isset($json['humidity'])) {
        $temperature = $json['temperature'];
        $humidity = $json['humidity'];

        // Inserir dados no banco de dados
        $stmt = $pdo->prepare('INSERT INTO readings (temperature, humidity) VALUES (?, ?)');
        $stmt->execute([$temperature, $humidity]);

        echo 'Dados armazenados com sucesso!';
    } else {
        echo 'Erro ao obter dados do ESP32';
    }
}

// Loop para salvar dados a cada 2 segundos
while (true) {
    saveData($pdo, $esp32_url);
    sleep(2);
    echo "novo entrada foi salva!";
}
?>
