<?php
header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json");

// Generate fake data
$temperature = rand(0, 50) + (rand(0, 99) / 100);
$humidity = rand(20, 80) + (rand(0, 99) / 100);

$data = array(
    'temperature' => $temperature,
    'humidity' => $humidity
);

echo json_encode($data);
?>

