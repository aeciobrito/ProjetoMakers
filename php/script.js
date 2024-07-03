document.addEventListener('DOMContentLoaded', function() {
    const temperatureElement = document.getElementById('temperature');
    const humidityElement = document.getElementById('humidity');

    function fetchData() {
        fetch('http://localhost/projetomakers/php/read_data.php')
            .then(response => response.json())
            .then(data => {
                temperatureElement.textContent = data.temperature;
                humidityElement.textContent = data.humidity;
            })
            .catch(error => console.error('Erro ao buscar dados:', error));
    }
    
    // Busca os dados a cada 10 segundos
    setInterval(fetchData, 10000);

    // Busca os dados imediatamente ao carregar a página
    fetchData();
});
