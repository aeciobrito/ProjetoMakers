document.addEventListener('DOMContentLoaded', function() {
    const temperatureElement = document.getElementById('temperature');
    const humidityElement = document.getElementById('humidity');
  
    function fetchData() {
      fetch('http://esp32.local/data')
        .then(response => response.json())
        .then(data => {
          temperatureElement.textContent = data.temperature;
          humidityElement.textContent = data.humidity;
        })
        .catch(error => console.error('Error fetching data:', error));
    }
  
    // Fetch data every 2 seconds
    setInterval(fetchData, 2000);
  
    // Fetch data immediately on load
    fetchData();
});
