<?php
header('Content-Type: application/json');

// Generate fake data
$data = array();
for ($i = 0; $i < 100; $i++) {
    $timestamp = date('Y-m-d H:i:s', strtotime("-$i minutes"));
    $temperature = rand(-10, 40) + (rand(0, 99) / 100);
    $humidity = rand(20, 80) + (rand(0, 99) / 100);
    $data[] = array(
        'temperature' => $temperature,
        'humidity' => $humidity,
        'timestamp' => $timestamp
    );
}

echo json_encode(array_reverse($data));
?>
