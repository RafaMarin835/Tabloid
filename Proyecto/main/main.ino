#include "AdafruitIO_WiFi.h"

#include "Humificador.h"
#include "NeoPixelModule.h"
#include "SensorModule.h"
#include "ButtonModule.h"
#include "DFPlayer8266.h"
#include "Modos.h" 


//-------------------Conexion con adafruit----------//
#define IO_USERNAME  "XXX" //Segun tu info de Adafruit, se debe respetar los Feed en AdaFruit
#define IO_KEY       "XXX"
#define WIFI_SSID    "ACC"
#define WIFI_PASS    "12345677"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

//-------------------Feed de Adafruit--------------//
AdafruitIO_Feed *NumMode = io.feed("NumMode");
AdafruitIO_Feed *ColorLuces = io.feed("ColorLuces");
AdafruitIO_Feed *Brillo = io.feed("Brillo");
AdafruitIO_Feed *Sonido = io.feed("Sonido");
AdafruitIO_Feed *EstadoHumificador = io.feed("EstadoHumificador");
AdafruitIO_Feed *IntervaloHumificador = io.feed("IntervaloHumificador");
AdafruitIO_Feed *DuracionHumificador = io.feed("DuracionHumificador");


//----------------Variables de estado y modos---------------//
int modoElegido = 0;
int Volumen = 15;
String ColorLed = "#000000"; //color de la led en hexadecimal
String EstadoHumificadorAr = "OFF"; // "ON" o "OFF"
String humidifierIntervalStr = "00:00:10"; // "hh:mm:ss" o vacío
String humidifierDurationStr = "00:00:05"; // "hh:mm:ss" o vacío

//----------------Declaracion de pines y o variables fijas---------------//
const int PIN_NEOPIXEL = 15; //Pin de led D8
const int NUMPIXELS = 5; //Numero de leds
const int PIN_ECHO = 5; //Pin D1
const int PIN_TRIG = 16; //Pin D0

const int PIN_BOTON = 14; //pin D5
int PIN_HUMIFICADOR = 2; //Pin D4
int PIN_HUMIFICADORON = 4; //Pin D2

const int Por_DEFINIR = 12; //pin D6
const int PIN_POR_DEFINIR2 = 0; //Pin D3
const int PIN_POR_DEFINIR3 = 4; //Pin D2

const int tener_cuidado = 13; //pin D7

// Reloj global
unsigned long tiempoActualMs = 0;
unsigned long tiempoAnteriorMs = 0;
unsigned long ticksGlobal = 0;

void setup() {
  Serial.begin(9600);

  initNeoPixel(PIN_NEOPIXEL, NUMPIXELS); //llamar a NeoPixelModules
  initSensor(PIN_TRIG, PIN_ECHO); //iniciar el sensor de proximidad
  initButton(PIN_BOTON, onButtonPress);// enviar inicio al boton 
  initHumificador(PIN_HUMIFICADOR, PIN_HUMIFICADORON); // iniciar el humificador

  // Inicializa DFPlayer usando pines------------------------------------------
  if (!DF8266::begin(&Serial)) {
    Serial.println("Error al iniciar DFPlayer");
  } else {
    Serial.println("DFPlayer iniciado correctamente");
  }

  //----------conexion y primera llamada de adafruit-------//
  io.connect();
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Conectado a Adafruit IO");

  //----------Llamadas en cada cambio que se hace desde adafruit
  NumMode->onMessage(handleNumMode);
  ColorLuces->onMessage(handleColorLuces);
  Brillo->onMessage(handleBrillo);
  Sonido->onMessage(handleSonido);
  EstadoHumificador->onMessage(handleEstadoHumificador);
  IntervaloHumificador->onMessage(handleIntervaloHumificador);
  DuracionHumificador->onMessage(handleDuracionHumificador);

  NumMode->save(modoElegido);
  delay(500);
  EstadoHumificador->save(EstadoHumificadorAr);
  delay(500);
  Sonido->save(Volumen);
}

void loop() {
  //+++++++++++++++++++++++Reloj++++++++++++++++++++++++++//
  tiempoActualMs = millis();
  if (tiempoAnteriorMs - tiempoActualMs >= 100) { // 100 ms por tick
    tiempoAnteriorMs = tiempoActualMs;
    ticksGlobal++;
  //+++++++++++++++++++++++++++++++++++++//

    //-----------------Llamada interna cada 100ms-----------------//
    
    // Llamada periódica para que el módulo gestione su estado y tiempos
    actualizarHumificador(ticksGlobal, EstadoHumificadorAr);
    tickSensor(ticksGlobal); //para que el sensor mida cada 100ms

  }

  Modo();

  DF8266::loop(&Serial);

  //-----------------Fuera del reloj, tareas no dependientes del relog
  tickButton();    // antirrebote

  io.run(); //matener conexion con adafruit

}

bool PuedeModoStamby = true;
bool PuedeModo1 = true;
bool PuedeModo2 = true;
bool PuedeModo3 = true;
bool PuedeModo4 = true;
void Modo(){
  if(modoElegido == 0) {
    Stamby(EstadoHumificadorAr, ticksGlobal, PuedeModoStamby);
    PuedeModoStamby = false;
    PuedeModo1 = true;
    PuedeModo2 = true;
    PuedeModo3 = true;
    PuedeModo4 = true;
  }

  else if (modoElegido == 1) {
    modo1(ticksGlobal, EstadoHumificadorAr, PuedeModo1); //modo1();
    PuedeModoStamby = true;
    PuedeModo1 = false;
    PuedeModo2 = true;
    PuedeModo3 = true;
    PuedeModo4 = true;
  }
  else if (modoElegido == 2) {
    modo2(ticksGlobal, EstadoHumificadorAr, PuedeModo2);
    PuedeModoStamby = true;
    PuedeModo1 = true;
    PuedeModo2 = false;
    PuedeModo3 = true;
    PuedeModo4 = true;
    }
  else if (modoElegido == 3) {
    Modo3(ticksGlobal, EstadoHumificadorAr, PuedeModo3);//modo3();
    PuedeModoStamby = true;
    PuedeModo1 = true;
    PuedeModo2 = true;
    PuedeModo3 = false;
    PuedeModo4 = true;
  }

  else if (modoElegido == 4) {
    modo4(ticksGlobal, ColorLed, humidifierIntervalStr, humidifierDurationStr, PuedeModo4);
    PuedeModoStamby = true;
    PuedeModo1 = true;
    PuedeModo2 = true;
    PuedeModo3 = true;
    PuedeModo4 = false;
  }
  else { modoElegido = 0; 
    PuedeModoStamby = true;
    PuedeModo1 = true;
    PuedeModo2 = true;
    PuedeModo3 = true;
    PuedeModo4 = true;
  }
  
}

// Callback para cuando se preciona el boton (registrado en ButtonModule)
void onButtonPress() {
  Serial.println("Botón: onButtonPress");

  NumMode->get();
  modoElegido++;
  if (modoElegido >= 5){
    modoElegido = 0;
  }
  NumMode->save(modoElegido);
}


// Llenar color re-exporta a NeoPixel module
void llenarColor(uint8_t r, uint8_t g, uint8_t b) {
  setNeoColor(r, g, b);
}

//*********************************************Las llamadas a adafruit***********************/////////
void handleNumMode(AdafruitIO_Data *data) {
  int modo = data->toInt();
  modoElegido = modo;
  Serial.println("Modo: " + modoElegido);
}

void handleColorLuces(AdafruitIO_Data *data) {
  String color = data->toString();
  ColorLed = color;
  Serial.println("Color " + color);
}

void handleBrillo(AdafruitIO_Data *data) {
  if (!data) return;
  int Brillo = data->toInt(); // si tu slider es 0..255 por ejemplo
  Serial.print("Brillo recibido: " + Brillo);
  setNeoBrillo(Brillo);
}

void handleSonido(AdafruitIO_Data *data) {
  if (!data) return;
  int Sonido = data->toInt(); // si tu slider es 0..255 por ejemplo
  Serial.print("Sonido recibido: " + Sonido);
  Volumen = Sonido;
  DF8266::volume(Volumen);
}

void handleEstadoHumificador(AdafruitIO_Data *data) {
  if (!data) return;
  String estado = data->toString();
  Serial.print("Estado humificador recibido: " + estado);
  EstadoHumificadorAr = estado;
}

void handleIntervaloHumificador(AdafruitIO_Data *data) {
  if (!data) return;
  String intervalo = data->toString();
  Serial.print("Intervalo del humificador recibido: " + intervalo);
  humidifierIntervalStr = intervalo;
}

void handleDuracionHumificador(AdafruitIO_Data *data) {
  if (!data) return;
  String duracion = data->toString();
  Serial.print("Duracion humificador recibido: " + duracion);
  humidifierDurationStr = duracion;
}

