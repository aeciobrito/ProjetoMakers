<?php
date_default_timezone_set('America/Sao_Paulo');
header('Content-Type: application/json');
require 'config.php';

//$esp32_url = 'http://esp32.local/data'; // URL do ESP32
$esp32_url = 'http://localhost/projetomakers/php/fake_data.php'; // URL dos dados mockados

// Obter dados do ESP32
$data = file_get_contents($esp32_url);
$json = json_decode($data, true);

$action = isset($_GET['action']) ? $_GET['action'] : '';

switch ($action) {
    case 'current':
        if (isset($json['temperature']) && isset($json['humidity'])) {            
            $temperature = $json['temperature'];
            $humidity = $json['humidity'];

            // Inserir dados no banco de dados
            $stmt = $pdo->prepare('INSERT INTO readings (temperature, humidity, reading_time) VALUES (?, ?, NOW())');
            $stmt->execute([$temperature, $humidity]);
    
            // Enviar resposta para o frontend
            echo json_encode(['temperature' => $temperature, 'humidity' => $humidity]);
        } else {
            echo json_encode(['error' => 'Erro ao obter dados do ESP32']);
        }
        break;
    case 'today':
        $today = date('Y-m-d');
        $stmt = $pdo->prepare('SELECT * FROM readings WHERE DATE_FORMAT(reading_time, "%Y-%m-%d") = :today ORDER BY reading_time ASC');
        $stmt->execute(['today' => $today]);
        $readings = $stmt->fetchAll(PDO::FETCH_ASSOC);

        if ($readings) {
            echo json_encode($readings);
        } else {
            echo json_encode(['error' => 'Nenhuma leitura encontrada para o dia de hoje']);
            echo $today;
        }
        break;
    case 'recomendations':
        $recommendation = [];
        $temperature = isset($_GET['temperature']) ? $_GET['temperature'] : null;
        $humidity = isset($_GET['humidity']) ? $_GET['humidity'] : null;

        if ($temperature !== null && $humidity !== null) {
            if ($temperature > 30) {
                $recommendation[] = 'A temperatura está muito alta. Certifique-se de se manter hidratado.';
            } elseif ($temperature < 10) {
                $recommendation[] = 'A temperatura está muito baixa. Mantenha-se aquecido.';
            } else {
                $recommendation[] = 'A temperatura está agradável.';
            }

            if ($humidity > 70) {
                $recommendation[] = 'A umidade está muito alta. Pode causar desconforto.';
            } elseif ($humidity < 30) {
                $recommendation[] = 'A umidade está muito baixa. Pode causar secura na pele.';
            } else {
                $recommendation[] = 'A umidade está em um nível confortável.';
            }
        } else {
            $recommendation[] = 'Dados de temperatura e umidade não fornecidos.';
        }

        echo json_encode($recommendation);
        break;
    default:
        echo json_encode(['error' => 'Ação inválida']);
}
?>
