function lerTemperatura() {
    // Conecta-se ao ESP32
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "http://esp32.local/temperatura");
    xhr.onload = function() {
      // Obtém a temperatura do ESP32
      var temperatura = xhr.responseText;
  
      // Exibe a temperatura
      document.getElementById("temperatura").innerHTML = temperatura;
  
      // Verifica se a temperatura atual é igual ou superior à temperatura alvo
      if (temperatura >= document.getElementById("temperaturaAlvo").value) {
        // Liga a lâmpada
        document.getElementById("lampada").style.backgroundColor = "#00ff00";
  
        // Envia uma solicitação HTTP ao ESP32 para ligar o relé
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "http://esp32.local/ligar");
        xhr.send();
      } else {
        // Desliga a lâmpada
        document.getElementById("lampada").style.backgroundColor = "#ffffff";
      }
    };
    xhr.send();
  }
  
  // Lê a temperatura a cada segundo
  setInterval(lerTemperatura, 1000);
  
  // Adiciona um evento de mudança ao elemento <input> com o ID "temperaturaAlvo"
  document.getElementById("temperaturaAlvo").addEventListener("change", function() {
    // Obtém a nova temperatura alvo
    var novaTemperaturaAlvo = document.getElementById("temperaturaAlvo").value;
  
    // Verifica se a temperatura atual é igual ou superior à nova temperatura alvo
    if (temperatura >= novaTemperaturaAlvo) {
      // Envia uma solicitação HTTP ao ESP32 para ligar o relé
      var xhr = new XMLHttpRequest();
      xhr.open("POST", "http://esp32.local/ligar");
      xhr.send();
    }
  });
  