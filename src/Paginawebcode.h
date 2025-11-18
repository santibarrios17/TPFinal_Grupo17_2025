const char Paginawebcode[] = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WebSocket Temperature</title>
    <style>
    body {
        background-color: #444; /* Fondo gris oscuro */
        font-family: Arial, sans-serif; /* Fuente */
        text-align: center; /* Centrar texto en el cuerpo */
        margin: 0;
        padding: 0;
    }

    h1, h2 {
        color: #eee; /* Color del texto */
    }

    #TEMPERATURA, #HUMEDAD {
        font-size: 24px; /* Tamaño de fuente para los valores de temperatura y humedad */
        font-weight: bold; /* Texto en negrita */
    }

    input[type="number"], button {
        padding: 10px;
        font-size: 16px; /* Tamaño de fuente para el cuadro de texto y botones */
        border: none;
        border-radius: 10px; /* Redondear las esquinas */
    }

    input[type="number"] {
        background-color: #666; /* Fondo gris oscuro para el cuadro de texto */
        color: #fff; /* Color del texto del cuadro de texto */
    }

    button {
        background-color: #0078d4; /* Color de fondo del botón */
        color: #fff; /* Color del texto del botón */
        cursor: pointer;
    }

    button:hover {
        background-color: #005bb8; /* Color de fondo del botón al pasar el cursor */
    }

    /* Estilos adicionales personalizables aquí */

</style>
</head>
<body>
    <h1>Temperatura: <span id="TEMPERATURA">0</span></h1>
    <h1>Humedad: <span id="HUMEDAD">0</span></h1>

    <h2>Eliga la temperatura para prende el ventilador:</h2>
    <input type="number" id="setpoint" min="0" step="0.1" value="0">
    <button onclick="setSetpoint()">Configurar</button>

    <h2>Eliga la temperatura para apagar la estufa:</h2>
    <input type="number" id="tem" min="0" step="0.1" value="0">
    <button onclick="setSetpointS()">Configurar</button>

    <h2>Setpoint de Hora:</h2>
    <input type="number" id="hora" min="0" step="0.1" value="0">
    <input type="number" id="minutos" min="0" step="0.1" value="0">
    <button onclick="setHora()">Configurar</button>

     <h2>Eliga a que hora quiere que la alarma se prenda:</h2>
    <input type="number" id="alarm" min="0" step="0.1" value="0">
    <button onclick="setAlarma()">Configurar</button>

    <h2>Control de Alarma:</h2>
    <button onclick="encenderAlarma()">Encender Alarma</button>
    <button onclick="fetch('/apagarAlarma')">Apagar alarma</button>


<script>

var socket = new WebSocket('ws://' + location.host + '/ws');

socket.onmessage = function(event) {
    var data = JSON.parse(event.data);

    document.getElementById('TEMPERATURA').textContent = data.temperatura;
    document.getElementById('HUMEDAD').textContent = data.humedad;

    if ("alarma" in data) {
        console.log("Estado de alarma recibido:", data.alarma);
    }
};

function setSetpoint() {
    var setpoint = parseFloat(document.getElementById('setpoint').value);
    socket.send(JSON.stringify({ setpoint: setpoint }));
}

function setSetpointS() {
    var tem = parseFloat(document.getElementById('tem').value);
    socket.send(JSON.stringify({ tem: tem }));
}

function setHora() {
    var hora = parseInt(document.getElementById('hora').value);
    var minutos = parseInt(document.getElementById('minutos').value);
    socket.send(JSON.stringify({ hora: hora, minutos: minutos }));
}

function setAlarma() {
    var alarm = parseFloat(document.getElementById('alarm').value);
    socket.send(JSON.stringify({ alarm: alarm }));
}

function encenderAlarma() {
    socket.send(JSON.stringify({ alarma: true }));
}

function apagarAlarma() {
    socket.send(JSON.stringify({ alarma: false }));
}

</script>

</body>
</html>
)=====";