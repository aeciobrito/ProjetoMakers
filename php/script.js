document.addEventListener('DOMContentLoaded', function() {
    const temperatureElement = document.getElementById('temperature');
    const humidityElement = document.getElementById('humidity');
    const ctx = document.getElementById('historyChart').getContext('2d');
    let historyChart;

    function fetchData() {
        //fetch('http://esp32.local/data')
        fetch('http://localhost/projetomakers/php/fake_data.php')
            .then(response => response.json())
            .then(data => {
                temperatureElement.textContent = data.temperature;
                humidityElement.textContent = data.humidity;
            })
            .catch(error => console.error('Erro ao buscar dados:', error));
    }

    function fetchHistory() {
        fetch('api.php?action=today')
            .then(response => response.json())
            .then(data => {
                const labels = data.map(entry => new Date(entry.reading_time).toLocaleTimeString());
                const temperatures = data.map(entry => entry.temperature);
                const humidities = data.map(entry => entry.humidity);

                if (historyChart) {
                    historyChart.destroy();
                }

                historyChart = new Chart(ctx, {
                    type: 'line',
                    data: {
                        labels: labels,
                        datasets: [
                            {
                                label: 'Temperature (°C)',
                                data: temperatures,
                                borderColor: 'rgba(202, 53, 23, 0.7)',
                                fill: false,
                                tension: 0.1
                            },
                            {
                                label: 'Humidity (%)',
                                data: humidities,
                                borderColor: 'rgba(0, 173, 214, 0.7)',
                                fill: false,
                                tension: 0.1
                            }
                        ]
                    },
                    options: {
                        responsive: true,
                        scales: {
                            x: {
                                type: 'time',
                                time: {
                                    unit: 'minute'
                                }
                            }
                        }
                    }
                });
            })
            .catch(error => console.error('Erro ao buscar histórico:', error));
    }

    // Busca os dados a cada 2 segundos, tempo de atualização do módulo
    setInterval(fetchData, 2000);

    // Busca os dados imediatamente ao carregar a página
    fetchData();

    // Busca o histórico imediatamente ao carregar a página
    fetchHistory();

    // Busca o histórico a cada 60 segundos
    setInterval(fetchHistory, 60000);
});
