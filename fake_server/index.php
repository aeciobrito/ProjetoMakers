<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Sensor Data History</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 20px;
    }
    table {
      width: 100%;
      border-collapse: collapse;
      margin-bottom: 20px;
    }
    table, th, td {
      border: 1px solid black;
    }
    th, td {
      padding: 10px;
      text-align: left;
    }
    th {
      background-color: #f2f2f2;
    }
    h1 {
      text-align: center;
    }
  </style>
  <script>
    async function fetchData() {
      try {
        const response = await fetch('http://localhost/aecio/ProjetoMakers/fake_server/mock_data.php');
        const data = await response.json();

        updateTable(data);
      } catch (error) {
        console.error('Error fetching data:', error);
      }
    }

    function updateTable(data) {
      const tableBody = document.getElementById('dataBody');
      tableBody.innerHTML = ''; // Limpa dados existentes

      data.forEach(entry => {
        const row = document.createElement('tr');

        const timestampCell = document.createElement('td');
        timestampCell.textContent = new Date(entry.timestamp).toLocaleString();
        row.appendChild(timestampCell);

        const temperatureCell = document.createElement('td');
        temperatureCell.textContent = entry.temperature.toFixed(2) + ' °C';
        row.appendChild(temperatureCell);

        const humidityCell = document.createElement('td');
        humidityCell.textContent = entry.humidity.toFixed(2) + ' %';
        row.appendChild(humidityCell);

        tableBody.appendChild(row);
      });
    }

    window.onload = function() {
      fetchData();
      setInterval(fetchData, 4000); // Busca dados a cada x segundos
    }
  </script>
</head>
<body>
  <h1>ESP32 Sensor Data History</h1>
  <table>
    <thead>
      <tr>
        <th>Timestamp</th>
        <th>Temperature (°C)</th>
        <th>Humidity (%)</th>
      </tr>
    </thead>
    <tbody id="dataBody">
      <!-- Data will be inserted here -->
    </tbody>
  </table>
</body>
</html>
