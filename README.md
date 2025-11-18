 Sistema de Control Ambiental con ESP32

Monitoreo de temperatura y humedad, control de ventilación/estufa, reloj interno y sistema de alarma con interfaz web.

 -Integrantes del Grupo-
Nombres y apellidos: Luciano Abuin y Santiago Barrios

Correos: lucianoabuin2@gmail.com y sabarrios02@gmail.com

 -Docente y Materia-

Materia: Laboratorio de computacion 1

Docente: Pedro Facundo Iriso Y Matias Jose Gagliardo

Este proyecto es básicamente un mini sistema inteligente de control ambiental, pensado para mantener una habitación dentro de un rango de temperatura cómodo.
El sistema:

Mide temperatura y humedad con un DHT11.
Muestra los datos en una pantalla LCD.
Enciende un motor (ventilador) si hace calor.
Enciende una estufa (simulada con un LED) si hace frío.
Tiene un reloj interno que cuenta horas, minutos y segundos.
Incluye una alarma con buzzer que se puede programar en una hora desde una página web.
Todo se controla y monitorea desde una interfaz web en tiempo real, gracias al WebSocket.

Es como un mini “climatizador” casero con todo integrado en la pagina web y con el muestreo en el panel lcd.
El sistema permite:

Monitorear temperatura y humedad.

Controlar automáticamente un ventilador y un calefactor (LED) según valores configurados.

Visualizar hora actual (reloj basado en millis).

Configurar una alarma horaria que activa un buzzer.

Ver y modificar parámetros desde una página web en tiempo real.

✔️ Requisitos Funcionales Cumplidos
-Sensado y Monitoreo

Lectura de temperatura y humedad con DHT11.

Visualización en LCD 16x2 I2C.

- Control Automático

Control de ventilador con PWM.

Activación/desactivación de estufa simulada con LED.

- Comunicación Web

Página web servida desde ESP32.

WebSocket para enviar datos en tiempo real sin recargar la página.

- Sistema de Alarma

Configuración horaria remota.

Buzzer activo solo cuando la hora coincide.

Apagado de alarma por web.

- Reloj interno

Implementado con millis(), sin uso de RTC externo.

🔧 Componentes Utilizados

ESP32

Sensor DHT11

LCD 16x2 con módulo I2C

Ventilador / Motor DC + Driver (puente H)

Buzzer

LED indicador

Cables Dupont y Protoboard

Fuente 5V o USB

 #Esquema Eléctrico / Diagrama de Conexiones
 Pines utilizados en el código:
Componente	Pin
DHT11	GPIO 27
LED / Estufa	GPIO 2
Motor VENT1	GPIO 16
Motor VENT2	GPIO 17
ENA (PWM)	GPIO 4
Buzzer	GPIO 33
LCD I2C	SDA/SCL (21 y 22)
Cómo usar este proyecto (explicado fácil)
1️⃣ Conectar todo
Seguir el esquema de pines que está arriba.
No tiene mucha ciencia: sensor → ESP32, LCD → SDA/SCL, ventilador al driver y el buzzer al pin 33.

2️⃣ Cargar el código
Abrí Arduino IDE → Seleccioná la placa ESP32 → Cargá el programa.

3️⃣ Conectarse al WiFi
El ESP32 se conecta automáticamente a:
Red: Natalia(este SSID podes modificarlo en el codigo, segun tu red wifi)
Contraseña: 69366936(esta "Password" podes modificarla en el codigo, segun tu red wifi)
Cuando arranca, muestra la IP en el Monitor Serie.

4️⃣ Abrir la página web

Entrá desde cualquier navegador a la IP que muestra el ESP32.
Ejemplo:

http://192.168.0.12

5️⃣ Desde la página web podés:

-Ver temperatura y humedad.
-Cambiar el setpoint del ventilador.
-Cambiar el setpoint de la estufa.
-Ajustar la hora interna
-Configurar la alarma y apagarla.

Todo responde al instante.
