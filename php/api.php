<?php
require 'config.php';

date_default_timezone_set('America/Sao_Paulo');

header('Content-Type: application/json');

$action = isset($_GET['action']) ? $_GET['action'] : '';

switch ($action) {
    case 'current':
        getCurrentReading($pdo);
        break;
    case 'today':
        getTodayReadings($pdo);
        break;
    default:
        echo json_encode(['error' => 'Ação inválida']);
}

function getCurrentReading($pdo) {
    $stmt = $pdo->query('SELECT * FROM readings ORDER BY reading_time DESC LIMIT 1');
    $reading = $stmt->fetch();

    if ($reading) {
        $response = [
            'temperature' => $reading['temperature'],
            'humidity' => $reading['humidity'],
            'recommendation' => getRecommendation($reading['temperature'], $reading['humidity']),
        ];
    } else {
        $response = [
            'error' => 'Nenhuma leitura encontrada'
        ];
    }

    echo json_encode($response);
}

function getTodayReadings($pdo) {
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
}


function getRecommendation($temperature, $humidity) {
    $recommendation = [];

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

    return $recommendation;
}
?>
