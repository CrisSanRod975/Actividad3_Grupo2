# Actividad3_Grupo2
# Esquemático del circuito
# Lista de componentes
## Componentes utilizados

| Componente | Función |
|---|---|
| Arduino UNO | Control principal del sistema |
| Keypad | Selección de planta por parte del usuario |
| Servomoto | Simulador del movimiento del ascensor |
| LCD 16x2 I2C | Visualización de parámetros |
| Tira de LEDs - NeoPixel | Indicador visual de la posición del ascensor |
| DHT22 | Sensor de temperatura y humedad |
| LEDs | Señalización visual |
| Buzzer | Alarmas acústicas |
| Pulsadores | Simulación de entrada y salidas de personas |
| Sensor PIR | Detección de presencia y activación automática |

---

## Descripción de componentes

### Arduino UNO
<img src="doc/img/arduino.png" width="220">

Microcontrolador principal encargado de la lectura de sensores, procesamiento de datos y control de actuadores del sistema automatizado. Seleccionado como unidad de control principal, debido a la facilidad de uso, compatibilidad con múltiples sensores y su integración con la plataforma de simulación WOKWI.

[Fuente de la imagen](https://en.wikipedia.org/wiki/Arduino_Uno)

[Datasheet](doc/datasheet/Arduino.pdf)

### Keypad (teclado matricial)
<img src="doc/img/keypad.jpg" width="220">

Utilizado para la selección de plantas por parte del usuario. Permite introducir las llamadas del ascensor de forma sencilla.

[Fuente de la imagen](https://components101.com/misc/4x4-keypad-module-pinout-configuration-features-datasheet)

[Datasheet](doc/datasheet/keypad.pdf)
### Servomotor
<img src="doc/img/servomotor.jpg" width="220">

Empleado para simular el movimiento del ascensor entre plantas mediante diferentes posiciones angulares.

[Fuente de la imagen](https://www.mikroe.com/micro-servo-motor-sg-180-degree)

[Datasheet](doc/datasheet/servomotor.pdf)

### Pantalla LCD 16x2 I2C
<img src="doc/img/lcdI2C.jpg" width="220">

Permite visualizar en tiempo real las variables ambientales, estados del sistema y parámetros configurables del ascensor industrial. Usado para la visualización de datos a través de una pantalla. Muestra de forma clara y organizada la información del sistema. El uso del protocolo I2C reduce el número de conexiones necesarias.

[Fuente de la imagen](https://tienda.bricogeek.com/pantallas-lcd/1444-pantalla-lcd-16x2-i2c.html)

[Datasheet](doc/datasheet/LCD.pdf)

### Tira de LEDs (NeoPixel)
<img src="doc/img/neopixel.png" width="220">

Utilizada como indicador visual de la posición del ascensor y de las llamadas pendientes. 

[Fuente de la imagen](https://shop.pimoroni.com/products/flexible-rgb-led-strip-neopixel-ws2812-sk6812-compatible?variant=30260032110675)

[Datasheet](doc/datasheet/neopixel.pdf)
### Sensor DHT22

<img src="doc/img/dht22.png" width="220">

Sensor digital utilizado para medir temperatura y humedad relativa del entorno de operación. Encargado de medir la temperatura y la humedad dentro del ascensor, permitiendo el control automático del sistema de ventilación. 

[Fuente de la imagen](https://cityos-air.readme.io/docs/4-dht22-digital-temperature-humidity-sensor)

[Datasheet](https://drive.google.com/viewerng/viewer?url=https://cdn-shop.adafruit.com/datasheets/Digital%2Bhumidity%2Band%2Btemperature%2Bsensor%2BAM2302.pdf)

### LEDs de señalización
Indicadores visuales utilizados para representar estados de funcionamiento, alarmas o activación de sistemas auxiliares. Empleados para la iluminación del interior del ascensor y como indicador del sistema de ventilación.

### Buzzer
Actuador acústico utilizado para generar alertas sonoras ante determinadas condiciones del sistema. Implementado como sistema de aviso acústico en caso de sobrecarga de personas en el ascensor.

### Pulsadores
Utilizados para simular la entrada (pulsador izquierdo) y la salida (pulsador derecho) de personas del ascensor, permitiendo llevar un conteo del número de ocupantes.

### Sensor PIR (Infrarrojo pasivo)
Utilizado para detectar movimiento en las proximidades del ascensor, activando la iluminación de forma automática incluso antes de que los usuarios entren. La selección de estos componentes se ha realizado buscando un equilibrio entre simplicidad, funcionalidad y adecuación a los objetivos del sistema.

# Descripción de la lógica del firmware
# Video o Gif animado del funcionamiento del circuito
# Preguntas frecuentes
