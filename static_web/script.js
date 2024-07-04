document.addEventListener('DOMContentLoaded', function() {
    const temperatureElement = document.getElementById('temperature');
    const humidityElement = document.getElementById('humidity');
  
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
  
    // Busca os dados a cada 2 segundos, tempo de atualização do módulo
    setInterval(fetchData, 2000);
  
    // Busca os dados imediatamente ao carregar a página
    fetchData();
});
