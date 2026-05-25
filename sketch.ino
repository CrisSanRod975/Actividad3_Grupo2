#include <Keypad.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>



/** ============================
 * VARIABLES
 * ============================ */
  //Variables para LCD

  int ultimaPlanta = -1;
  int ultimasPersonas = -1;

  float ultimaTemp = -100;
  float ultimaHum = -100;

  String ultimoMensaje = "";
  bool ultimaVentilacion = false;

  //Keypad - Teclado para pulsar la planta a la que queremos ir
  bool llamadas[5] = {false, false, false, false, false};
  int plantaActual = 0;
  int direccion = 1; //1 =sube, -1 = baja 
  unsigned long tiempoAnterior = 0;
  const int intervalo = 2000; // 2 segundo por planta
  unsigned long tiempoPuertas = 0;
  const int tiempoEspera = 3000; // 5 segundos
  String mensajeAscensor= " ";

  enum Estado {
    MOVIENDO,
    PUERTAS_ABRIENDO,
    PUERTAS_ABIERTAS,
    PUERTAS_CERRANDO
  };

  Estado estado = MOVIENDO;
  const byte filas = 2;
  const byte columnas = 3;

  char teclas[filas][columnas] = {
    {'1','2','3'},
    {'4','5','6'}
  };

  byte pinesFilas[filas] = {9, 8};
  byte pinesColumnas[columnas] = {7, 6, 5};

  Keypad keypad = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, filas, columnas);

  //Servomotor - Control del movimiento del ascensor
  Servo myservo; 
  int plantaAnteriorServo = -1;

  //Led para llamada
  #define PIN 12
  #define NUMPIXELS 5

  Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

  // Contador de personas con pulsadores

  int personas = 0;
  int botonEntrada = A1; 
  int botonSalida = A0; 

  //Buzzer de sobrecarga
  #define BUZZER_PIN 4
  int maxPersonas = 4;

  // LCD
  LiquidCrystal_I2C lcd (0x27, 20, 4);
  byte personaIcono[8] = {
    B00100,
    B01110,
    B00100,
    B01110,
    B10101,
    B00100,
    B01010,
    B10001
  };

  //Sensor de temperatura y humedad
    #define DHTTYPE DHT22
    #define DHTPIN 2 
    float h;
    float t;
    DHT dht(DHTPIN, DHTTYPE);
    bool ventilacionActiva = false;

  //PIR - Ventilacion y luz automatica
  #define PIR_PIN 3
  #define LED_LUZ 13
  #define VENTILADOR 10

/** ============================
 * FUNCIONES
 * ============================ */

  /** ============================
  *  FUNCIÓN: moverMotor
  *  Descripción:Posiciona el servomotor según la planta actual,
  *  simulando el movimiento del ascensor entre plantas.
  * ============================ */
   void moverMotor(int planta){
      int angulo = 0; 
      switch(planta){
        case 0: angulo = 0; break;
        case 1: angulo = 45; break; 
        case 2: angulo = 90; break; 
        case 3: angulo = 135; break; 
        case 4: angulo = 180; break;  

      }
    myservo.write(angulo);
    }
  /** ============================
  *  FUNCIÓN: leerKeypad
  *  Descripción: Lee la entrada del teclado matricial y registra
  *  llamadas a plantas. Si se pulsa la planta actual, abre directamente las puertas.
  * ============================ */
  void leerKeypad() {
    char tecla = keypad.getKey();

    if (tecla != NO_KEY) {

      // Solo aceptar teclas válidas
      if (tecla >= '1' && tecla <= '5') {
        int planta = tecla - '1'; // convierte '1'→0, '2'→1...
        if (plantaActual != planta) {
          llamadas[planta] = true;
        } else {
        mensajeAscensor = "Puertas abiertas    ";
        estado = PUERTAS_ABRIENDO;
        tiempoPuertas = millis();
        }

      }
    }
  }

  /** ============================
  *  FUNCIÓN: siguientePlanta
  *  Descripción: Determina la siguiente planta destino en función
  *  de la dirección actual del ascensor.Si no hay llamadas en esa dirección, 
  *  invierte sentido.
  * ============================ */

  int siguientePlanta() {

    // Buscar en la dirección actual
    if (direccion == 1) {
      for (int i = plantaActual + 1; i < 5; i++) {
        if (llamadas[i]) return i;
      }
    } else {
      for (int i = plantaActual - 1; i >= 0; i--) {
        if (llamadas[i]) return i;
      }
    }

    // Si no hay → cambiar dirección
    direccion = -direccion;

    // Buscar otra vez
    if (direccion == 1) {
      for (int i = plantaActual + 1; i < 5; i++) {
        if (llamadas[i]) return i;
      }
    } else {
      for (int i = plantaActual - 1; i >= 0; i--) {
        if (llamadas[i]) return i;
      }
    }

    return -1; // no hay llamadas
  }

  /** ============================
  *  FUNCIÓN: moverAscensor
  *  Descripción: Controla el movimiento del ascensor mediante una máquina de estados:
  *  - MOVIENDO
  *  - PUERTAS_ABRIENDO
  *  - PUERTAS_ABIERTAS
  *  - PUERTAS_CERRANDO
  *  Gestiona tiempos, cambios de planta y estados.
  * ============================ */
  void moverAscensor() {

    if (personas > maxPersonas) return; 
    unsigned long tiempoActual = millis();

    switch (estado) {

      case MOVIENDO: {
        if (tiempoActual - tiempoAnterior >= intervalo) {

          tiempoAnterior = tiempoActual;

          int destino = siguientePlanta();

          if (destino != -1) {

            if (destino > plantaActual) {
              plantaActual++;
              if (plantaAnteriorServo != plantaActual) {
                myservo.attach(11);
                moverMotor(plantaActual);
                delay(200);          // pequeño tiempo para que llegue
                myservo.detach();
                plantaAnteriorServo = plantaActual;
              }
              mensajeAscensor = "Subiendo ^          ";
            } 
            else if (destino < plantaActual) {
              plantaActual--;
              if (plantaAnteriorServo != plantaActual) {
                myservo.attach(11);
                moverMotor(plantaActual);
                delay(200);          // pequeño tiempo para que llegue
                myservo.detach();
                plantaAnteriorServo = plantaActual;
            }
              mensajeAscensor = "Bajando v           ";
            }

            if (plantaActual == destino) {
              llamadas[destino] = false;
              estado = PUERTAS_ABRIENDO;
              tiempoPuertas = tiempoActual;
            }
          }
        }
        break;
      }

      case PUERTAS_ABRIENDO:
        mensajeAscensor = "Puertas abiertas    ";
        estado = PUERTAS_ABIERTAS;
        tiempoPuertas = tiempoActual;
        break;

      case PUERTAS_ABIERTAS:
        if (tiempoActual - tiempoPuertas >= tiempoEspera) {
          estado = PUERTAS_CERRANDO;
          tiempoPuertas = tiempoActual;
        }
        break;

      case PUERTAS_CERRANDO:
        mensajeAscensor = "Puertas cerradas    ";
        estado = MOVIENDO;
        tiempoAnterior = tiempoActual;
        break;
      }
  }

  /** ============================
  *  FUNCIÓN: actualizarLEDs
  *  Descripción:Actualiza la tira NeoPixel:
  *  - Verde → planta actual con puertas abiertas
  *  - Naranja → posición actual
  *  - Amarillo → llamadas pendientes
  *  - Apagado → sin actividad
  * ============================ */

  void actualizarLEDs() {

    for (int i = 0; i < 5; i++) {

      if (i == plantaActual && (estado == PUERTAS_ABRIENDO || estado == PUERTAS_ABIERTAS)) {
        // Verde → estás aquí
        pixels.setPixelColor(i, pixels.Color(0, 255, 0));
      }
      else if(i == plantaActual) {
        // Naranja → donde esta el ascensor en ese momento
        pixels.setPixelColor(i, pixels.Color(255, 100, 0));
      }
      else if (llamadas[i]) {
        // Amarillo → llamada
        pixels.setPixelColor(i, pixels.Color(255, 255, 0));
      }
      else {
        // Apagado
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      }
    }

    pixels.show();
  }

  /** ============================
  *  FUNCIÓN: pulsadorPersonas
  *  Descripción: Gestiona el conteo de personas mediante dos
  *  pulsadores (entrada/salida).Solo funciona cuando las puertas están abiertas.
  *  Incluye antirrebote para evitar conteos erróneos.
  * ============================ */

  void pulsadorPersonas(){

    if (estado == PUERTAS_ABIERTAS) {

    static int entradaAnterior = HIGH;
    static int salidaAnterior = HIGH;

    static unsigned long ultimoTiempoEntrada = 0;
    static unsigned long ultimoTiempoSalida = 0;

    const int debounce = 200; // ms

    int entradaActual = digitalRead(botonEntrada);
    int salidaActual = digitalRead(botonSalida);

    unsigned long tiempoActual = millis();

    // ENTRADA
    if (entradaAnterior == HIGH && entradaActual == LOW) {
      if (tiempoActual - ultimoTiempoEntrada > debounce) {
        personas++;
        ultimoTiempoEntrada = tiempoActual;
      }
    }

    // SALIDA
    if (salidaAnterior == HIGH && salidaActual == LOW) {
      if (tiempoActual - ultimoTiempoSalida > debounce) {
        if (personas > 0) personas--;
        ultimoTiempoSalida = tiempoActual;
      }
    }

    entradaAnterior = entradaActual;
    salidaAnterior = salidaActual;
    }
  }

  /** ============================
  *  FUNCIÓN: controlSobrecarga
  *  Descripción: Activa una alarma si se supera el número máximo
  *  de personas permitido. Mantiene las puertas abiertas hasta que la
  *  situación se normaliza.
  * ============================ */
  void controlSobrecarga() {

    if (personas > maxPersonas) {
      tone(BUZZER_PIN, 500); // pitido
      estado = PUERTAS_ABIERTAS; // Forzar puertas abiertas
    } 
    else {
      noTone(BUZZER_PIN); // parar pitido
    }
  }

  /** ============================
  *  FUNCIÓN: tempYhumedad
  *  Descripción:Lee los valores de temperatura y humedad del sensor DHT22.
  * ============================ */
  void tempYhumedad(){
    h = dht.readHumidity();
    t = dht.readTemperature();
  }

  /** ============================
  *  FUNCIÓN: datosLCD
  *  Descripción: Muestra en la pantalla LCD:
  *  - Planta actual
  *  - Número de personas
  *  - Estado del ascensor
  *  - Temperatura y humedad
  *  - Estado de la ventilación
  * ============================ */
void datosLCD(){


  // Línea 0 -> Planta y personas
  if(plantaActual != ultimaPlanta || personas != ultimasPersonas){

    lcd.setCursor (0,0);
    lcd.print("Planta:");
    lcd.print(plantaActual+1);
    lcd.print("  ");

    lcd.write(byte(0));

    lcd.print(":");
    lcd.print(personas);
    lcd.print("   ");

    ultimaPlanta = plantaActual;
    ultimasPersonas = personas;
  }

  // Línea 1 -> Estado ascensor
  if(mensajeAscensor != ultimoMensaje){

    lcd.setCursor (0,1);
    lcd.print("                    "); // Limpia línea
    lcd.setCursor (0,1);
    lcd.print(mensajeAscensor);

    ultimoMensaje = mensajeAscensor;
  }

  // Línea 2 -> Temperatura y humedad
  if(t != ultimaTemp || h != ultimaHum){

    lcd.setCursor (0,2);

    lcd.print("H:");
    lcd.print(h,1);
    lcd.print("% ");

    lcd.print("T:");
    lcd.print(t,1);

    lcd.write(223); // Símbolo °
    lcd.print("C   ");

    ultimaTemp = t;
    ultimaHum = h;
  }

  // Línea 3 -> Ventilación
  if(ventilacionActiva != ultimaVentilacion){

    lcd.setCursor (0,3);

    if (ventilacionActiva) {

      lcd.print("Ventilacion: ON     ");

    } else {

      lcd.print("Ventilacion: OFF    ");
    }

    ultimaVentilacion = ventilacionActiva;
  }
}

  /** ============================
  *  FUNCIÓN: controlAmbiente
  *  Descripción: Control inteligente del entorno del ascensor:
  *  - Luz:
  *    Se activa si hay personas o movimiento
  *  - Ventilación:
  *    Se activa si:temperatura > 25°C o humedad > 80% y hay personas en el ascensor.
  * ============================ */
  void controlAmbiente() {

    static unsigned long ultimoMovimiento = 0;
    const int tiempoActivo = 5000;

    // Detectar movimiento con PIR
    if (digitalRead(PIR_PIN) == HIGH) {
      ultimoMovimiento = millis();
    }

    bool hayMovimiento = (millis() - ultimoMovimiento < tiempoActivo);

    // =========================
    // CONTROL DE LUZ
    // =========================
    if (personas > 0 || hayMovimiento) {
      digitalWrite(LED_LUZ, HIGH);
    } else {
      digitalWrite(LED_LUZ, LOW);
    }
    // =========================
    // CONTROL DE VENTILACIÓN (con histéresis)
    // =========================
    // ENCENDER si sale del rango
  
    if (!ventilacionActiva && (t > 26 || t < 19 || h > 85)) {
      ventilacionActiva = true;
      }
    // APAGAR
    if (ventilacionActiva && (t > 21 && t < 24 && h < 80)) {
      ventilacionActiva = false;
    }
      if (ventilacionActiva && personas > 0) {
        digitalWrite(VENTILADOR, HIGH);
      } else {
        digitalWrite(VENTILADOR, LOW);
      }
    
  }

  /** ============================
  *  SETUP
  *  Descripción:
  *  Inicialización de todos los componentes:
  *  - Servo
  *  - LEDs
  *  - LCD
  *  - Sensor DHT22
  *  - PIR
  *  - Pines de entrada/salida
  * ============================ */
  void setup() {
    myservo.attach(11);
    moverMotor(plantaActual);//Empieza en planta actual
    pixels.begin();
    pixels.clear();
    pixels.show();
    pinMode(botonEntrada, INPUT_PULLUP);
    pinMode(botonSalida, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, personaIcono);
    dht.begin();
    pinMode(PIR_PIN, INPUT);
    pinMode(LED_LUZ, OUTPUT);
    digitalWrite(LED_LUZ, LOW);
    pinMode(VENTILADOR, OUTPUT);
    digitalWrite(VENTILADOR, LOW);
  }
  /** ============================
  *  LOOP
  *  Descripción:
  *  Bucle principal del sistema.
  *  Ejecuta de forma continua:
  *  - Lectura de entradas
  *  - Control del ascensor
  *  - Actualización de sensores
  *  - Visualización y automatización
  * ============================ */
  void loop() {
    leerKeypad();
    moverAscensor();
    actualizarLEDs();
    pulsadorPersonas();
    controlSobrecarga();
    tempYhumedad();
    datosLCD();
    controlAmbiente();
  }
