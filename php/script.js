document.addEventListener('DOMContentLoaded', function() {
    const temperatureElement = document.getElementById('temperature');
    const humidityElement = document.getElementById('humidity');
    const recommendationElement = document.getElementById('recommendation'); // Element to display recommendations

    function fetchData() {
        fetch('http://localhost/projetomakers/php/api.php?action=current')
            .then(response => response.json())
            .then(data => {
                temperatureElement.textContent = data.temperature;
                humidityElement.textContent = data.humidity;
                fetchRecommendations(data.temperature, data.humidity); // Fetch recommendations based on fetched data
            })
            .catch(error => console.error('Erro ao buscar dados:', error));
    }

    function fetchRecommendations(temperature, humidity) {
        fetch(`http://localhost/projetomakers/php/api.php?action=recomendations&temperature=${temperature}&humidity=${humidity}`)
            .then(response => response.json())
            .then(recommendations => {
                displayRecommendations(recommendations); // Display recommendations on the frontend
            })
            .catch(error => console.error('Erro ao buscar recomendações:', error));
    }

    function displayRecommendations(recommendations) {
        recommendationElement.innerHTML = ''; // Clear previous recommendations

        if (recommendations.length > 0) {
            recommendations.forEach(rec => {
                const p = document.createElement('p');
                p.textContent = rec;
                recommendationElement.appendChild(p);
            });
        } else {
            const p = document.createElement('p');
            p.textContent = 'Nenhuma recomendação disponível.';
            recommendationElement.appendChild(p);
        }
    }

    // Fetch data every 10 seconds
    setInterval(fetchData, 10000);

    // Fetch data immediately on page load
    fetchData();
});
