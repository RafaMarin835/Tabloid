#ifndef MODOS_H
#define MODOS_H

#include <Arduino.h>

#include "NeoPixelModule.h"
#include "Humificador.h"
#include "SensorModule.h"
#include "DFPlayer8266.h"





//-----------------*****************************------------------------Modo stamby-----------------------------***********************************--------------//
bool encendido = false;
void Stamby(String estado, unsigned long ticks, bool acepta) {
  setNeoColor(255, 210, 0);

  if (acepta){
    if (DF8266::isReady()) {
      DF8266::play(9);
    }
  }
  
  if (estado == "ON") {
    setEstadoHumificador(true);
    encendido = true;
  }
  else{
    setEstadoHumificador(false);
    encendido = false;
    }
}

//-----------------*****************************------------------------Modo 1-----------------------------***********************************--------------//
//----------------------------Para Modo 1---------------------------------//
static const unsigned long M1_AZUL_TICK = 150;   // 15 s
static const unsigned long M1_MORADA_TICK = 300; // 30 s
static const unsigned long M1_VERDE_TICK = 500;  // 50 s
static const unsigned long M1_HUM_INTERVAL = 200; // 20 s
static const unsigned long M1_HUM_DURATION = 50;  // 5 s

static bool modo1_inited = false;
static unsigned long modo1_startTick = 0;

static unsigned long modo1_lastAzulStart = 0;
static bool modo1_azulOn = false;

static unsigned long modo1_lastMoradaStart = 0;
static bool modo1_moradaOn = false;

static unsigned long modo1_lastVerdeStart = 0;
static bool modo1_verdeOn = false;

static unsigned long modo1_lastHumStart = 0;
static unsigned long modo1_humEndTick = 0;

static const float M1_SENSOR_THRESHOLD_CM = 10.0f;

enum LuzActiva { NINGUNA, AZUL, MORADA, VERDE };
static LuzActiva ultimaLuz = NINGUNA;

//-----------------------------------------Funciones auxiliares-------------------------------------------//
inline void mostrarColor(uint8_t r, uint8_t g, uint8_t b, LuzActiva luz) {
  setNeoColor(r, g, b);
  ultimaLuz = luz;
}

inline void restaurarColorBase() {
  if (!modo1_azulOn && !modo1_moradaOn && !modo1_verdeOn) {
    setNeoColor(255, 50, 100);
    ultimaLuz = NINGUNA;
  }
}

inline void modo1_init_if_needed(unsigned long ticks) {
  if (modo1_inited) return;
  modo1_inited = true;
  modo1_startTick = ticks;
  modo1_lastAzulStart = ticks;
  modo1_lastMoradaStart = ticks;
  modo1_lastVerdeStart = ticks;
  modo1_lastHumStart = ticks;
  modo1_azulOn = false;
  modo1_moradaOn = false;
  modo1_verdeOn = false;
  modo1_humEndTick = 0;
  setNeoColor(255, 50, 100);
  ultimaLuz = NINGUNA;
}

//-----------------------------------------Modo 1-------------------------------------------//
bool Amomento = true;
bool Rmomento = true;
bool Pmomento = true;
inline void modo1(unsigned long ticks, String humificadorActivo, bool acepta) {
  modo1_init_if_needed(ticks);
  float distancia = getDistanciaCm();
  
  if (acepta){
    if (DF8266::isReady()) {
      DF8266::play(1);
    }
    setEstadoHumificador(false);
  }
  
  // ---------- Humificador ----------
  if (humificadorActivo == "ON"){
    if (estaHumificadorEncendido()) {
      if (ticks >= modo1_humEndTick) {
        setEstadoHumificador(false);
        modo1_lastHumStart = ticks;
      }
    } else {
      if (ticks - modo1_lastHumStart >= M1_HUM_INTERVAL) {
          setEstadoHumificador(true);
          modo1_humEndTick = ticks + M1_HUM_DURATION;
      }
    }
  }

  // ---------- Luz Azul ----------
  if (!modo1_azulOn && (ticks - modo1_lastAzulStart >= M1_AZUL_TICK)) {
    if (Amomento){
      if (DF8266::isReady()) {
          DF8266::play(4);
        }
        Amomento = false;
        Rmomento = true;
        Pmomento = true;
    }
    mostrarColor(0, 0, 255, AZUL);
    modo1_azulOn = true;
  } else if (modo1_azulOn && distancia < M1_SENSOR_THRESHOLD_CM) {
    modo1_azulOn = false;
    modo1_lastAzulStart = ticks;
    restaurarColorBase();
  }

  // ---------- Luz Morada ----------
  if (!modo1_moradaOn && (ticks - modo1_lastMoradaStart >= M1_MORADA_TICK)) {
    if (Rmomento){
      if (DF8266::isReady()) {
          DF8266::play(5);
        }
        Amomento = true;
        Rmomento = false;
        Pmomento = true;
    }
    mostrarColor(255, 0, 230, MORADA);
    modo1_moradaOn = true;
  } else if (modo1_moradaOn && distancia < M1_SENSOR_THRESHOLD_CM) {
    modo1_moradaOn = false;
    modo1_lastMoradaStart = ticks;
    restaurarColorBase();
  }

  // ---------- Luz Verde ----------
  if (!modo1_verdeOn && (ticks - modo1_lastVerdeStart >= M1_VERDE_TICK)) {
    if (Pmomento){
      if (DF8266::isReady()) {
          DF8266::play(6);
        }
        Amomento = true;
        Rmomento = true;
        Pmomento = false;
    }
    mostrarColor(0, 255, 0, VERDE);
    modo1_verdeOn = true;
  } else if (modo1_verdeOn && distancia < M1_SENSOR_THRESHOLD_CM) {
    modo1_verdeOn = false;
    modo1_lastVerdeStart = ticks;
    restaurarColorBase();
  }
}


//-----------------*****************************------------------------Modo 2-----------------------------***********************************--------------//
//-----------------------------Para modo 2 relajación----------------------------//
// ----------------- Configuración en ticks (1 tick = 100 ms) -----------------
static const unsigned long M2_HUM_INTERVAL_TICKS = 150; // 15 s
static const unsigned long M2_HUM_DURATION_TICKS = 50;  // 5 s

static const unsigned long M2_START_DELAY_TICKS = 20;   // 2 s
static const unsigned long INHALAR_TICKS = 40;          // 4 s
static const unsigned long EXHALAR_TICKS = 60;          // 6 s
static const unsigned long PAUSA_TICKS = 100;           // 10 s

static const float M2_BREATH_MIN = 0.0f;
static const float M2_BREATH_MAX = 1.0f;

// ----------------- Variables internas -----------------
static unsigned long modo2_lastHumStart = 0;
static bool modo2_humActivo = false;
static unsigned long modo2_humEndTick = 0;

static unsigned long modo2_phaseTick = 0;
static uint8_t modo2_phaseType = 0; // 0 = inhalar, 1 = exhalar, 2 = pausa
static bool modo2_started = false;
static unsigned long modo2_startTick = 0;

// Colores definidos
static const uint8_t modo2_colorInR = 0, modo2_colorInG = 0, modo2_colorInB = 255;   // Azul suave
static const uint8_t modo2_colorOutR = 128, modo2_colorOutG = 0, modo2_colorOutB = 128; // Morado suave

// ----------------- Utilidades -----------------
inline float lerpF(float a, float b, float t){
  return a + (b - a) * t;
}

inline void lerpColor(uint8_t aR, uint8_t aG, uint8_t aB,
                      uint8_t bR, uint8_t bG, uint8_t bB,
                      float t,
                      uint8_t &outR, uint8_t &outG, uint8_t &outB){
  outR = (uint8_t)round(lerpF(aR, bR, t));
  outG = (uint8_t)round(lerpF(aG, bG, t));
  outB = (uint8_t)round(lerpF(aB, bB, t));
}

// ----------------- Inicialización -----------------
inline void modo2_init_if_needed(unsigned long ticks){
  static bool inited = false;
  if (inited) return;
  inited = true;
  modo2_lastHumStart = ticks;
  modo2_humActivo = false;
  modo2_humEndTick = 0;
  modo2_startTick = ticks;
  modo2_started = false;
  modo2_phaseTick = 0;
  modo2_phaseType = 0;
}

// ----------------- Modo 2 relajación -----------------
bool Imomento = true;
bool Emomento = true;
inline void modo2(unsigned long ticks, String humificadorON, bool acepta){
  modo2_init_if_needed(ticks);
  if (acepta){
    if (DF8266::isReady()) {
      DF8266::play(2);
    }
    setEstadoHumificador(false);
  }

  // ---------------- Humificador ----------------
  if (humificadorON =="ON") {
    if (modo2_humActivo) {
      if (ticks >= modo2_humEndTick) {
        setEstadoHumificador(false);
        modo2_humActivo = false;
        modo2_lastHumStart = ticks;
      }
    } 
    else {
      if (ticks - modo2_lastHumStart >= M2_HUM_INTERVAL_TICKS) {
        setEstadoHumificador(true);
        modo2_humActivo = true;
        modo2_humEndTick = ticks + M2_HUM_DURATION_TICKS;
        modo2_lastHumStart = ticks;
      }
    }
  } else {
    setEstadoHumificador(false);
    modo2_humActivo = false;
  }

  // ---------------- Espera inicial ----------------
  if (!modo2_started) {
    if (ticks - modo2_startTick >= M2_START_DELAY_TICKS) {
      modo2_started = true;
      modo2_phaseTick = ticks;
      modo2_phaseType = 0;
    } else {
      setNeoColor(0, 0, 0); // apagado durante espera
      return;
    }
  }

  // ---------------- Respiración ----------------
  unsigned long phaseDuration = 1;
  uint8_t fromR = 0, fromG = 0, fromB = 0;
  uint8_t toR = 0, toG = 0, toB = 0;
 

  if (modo2_phaseType == 0) { // Inhalar
    if (Imomento){
        if (DF8266::isReady()) {
          DF8266::play(7);
        }
        Imomento = false;
        Emomento = true;
      }
    
    fromR = modo2_colorOutR; fromG = modo2_colorOutG; fromB = modo2_colorOutB;
    toR = modo2_colorInR; toG = modo2_colorInG; toB = modo2_colorInB;
    phaseDuration = INHALAR_TICKS;
  } 
  else if (modo2_phaseType == 1) { // Exhalar
    if (Emomento){
        if (DF8266::isReady()) {
          DF8266::play(8);
        }
        Imomento = true;
        Emomento = false;
      }
      
    fromR = modo2_colorInR; fromG = modo2_colorInG; fromB = modo2_colorInB;
    toR = modo2_colorOutR; toG = modo2_colorOutG; toB = modo2_colorOutB;
    phaseDuration = EXHALAR_TICKS;
  } else { // Pausa
    setNeoColor(0, 0, 0);
    if (ticks - modo2_phaseTick >= PAUSA_TICKS) {
      modo2_phaseType = 0;
      modo2_phaseTick = ticks;
    }
    return;
  }

  // Cambio de fase
  if (ticks - modo2_phaseTick >= phaseDuration) {
    modo2_phaseType = (modo2_phaseType + 1) % 3;
    modo2_phaseTick = ticks;
  }

  // Progreso de fase
  float tPhase = float(ticks - modo2_phaseTick) / float(phaseDuration);
  if (tPhase > 1.0f) tPhase = 1.0f;

  // Color y brillo
  uint8_t baseR, baseG, baseB;
  lerpColor(fromR, fromG, fromB, toR, toG, toB, tPhase, baseR, baseG, baseB);

  float brightness = (modo2_phaseType == 0)
    ? lerpF(M2_BREATH_MAX, M2_BREATH_MIN, tPhase) // inhalar: 100% → 0%
    : lerpF(M2_BREATH_MIN, M2_BREATH_MAX, tPhase); // exhalar: 0% → 100%

  uint8_t outR = (uint8_t)round(baseR * brightness);
  uint8_t outG = (uint8_t)round(baseG * brightness);
  uint8_t outB = (uint8_t)round(baseB * brightness);

  setNeoColor(outR, outG, outB);
}


//-----------------*****************************------------------------Modo 3-----------------------------***********************************--------------//
//-----------------------------Para modo 3---------------------//
// Variables internas del módulo modo3 (visibilidad por header: static evita colisiones)
static unsigned long modo3_lastStartTick = 0;
static bool modo3_humActivo = false;
static unsigned long modo3_humEndTick = 0;

// Configuración en ticks (1 tick = 100 ms en tu loop)
static const unsigned long MODO3_INTERVAL_TICKS = 300; // 30 s
static const unsigned long MODO3_DURATION_TICKS = 50;  // 5 s

//----------------------------------------Modo 3----------------------------------//
inline void Modo3(unsigned long ticks, String estadoHumificadorON, bool acepta) {
  // Luz cálida constante
  setNeoColor(255, 210, 0);
  if (acepta){
    if (DF8266::isReady()) {
      DF8266::play(3);
    }
    setEstadoHumificador(false);
  }

  // Si el modo está desactivado por estado externo, apagamos humidificador si estaba activo
  if (estadoHumificadorON == "OFF") {
    if (modo3_humActivo) {
      setEstadoHumificador(false);
      modo3_humActivo = false;
    }
    return;
  }

  // Si el humidificador está activo, comprobamos si ya debe detenerse
  if (modo3_humActivo && ticks >= modo3_humEndTick) {
    setEstadoHumificador(false);
    modo3_humActivo = false;
  }

  // Si no está activo, comprobamos si corresponde iniciar (cada 30 s)
  if (!modo3_humActivo && (ticks - modo3_lastStartTick >= MODO3_INTERVAL_TICKS)) {
      setEstadoHumificador(true);
      modo3_humActivo = true;
      modo3_humEndTick = ticks + MODO3_DURATION_TICKS;
      modo3_lastStartTick = ticks;
  }
}


//-----------------*****************************------------------------Modo 4-----------------------------***********************************--------------//
//-------------funciones modo 4-----------------//
unsigned long parseTimeString(const String& timeStr) {
    int h = 0, m = 0, s = 0;
    sscanf(timeStr.c_str(), "%d:%d:%d", &h, &m, &s);
    return (h * 3600UL + m * 60UL + s) * 10UL;
}

bool parseHexColor(const String& hex, int& r, int& g, int& b) {
    if (hex.length() != 7 || hex.charAt(0) != '#') return false;

    r = strtol(hex.substring(1, 3).c_str(), nullptr, 16);
    g = strtol(hex.substring(3, 5).c_str(), nullptr, 16);
    b = strtol(hex.substring(5, 7).c_str(), nullptr, 16);

    return true;
}

//------------------modo 4--------------------//
static unsigned long lastHumidifierTick = 0;
void modo4( unsigned long tick, const String& ledHexColor, const String& humidifierIntervalStr, 
          const String& humidifierDurationStr, bool acepta) 
          {

    // === Evento de aceptación ===
    if (acepta) {
        if (DF8266::isReady()) {
            DF8266::play(10);
        }
        setEstadoHumificador(false);
    }

    // === LED Control ===
    int r, g, b;
    if (parseHexColor(ledHexColor, r, g, b)) {
        setNeoColor(r, g, b);  // Aplica color directamente
    } else {
        setNeoColor(0, 0, 0);  // Fallback si el color es inválido
    }

    // === Humidifier Control ===
        unsigned long intervaloTicks = parseTimeString(humidifierIntervalStr);
        unsigned long duracionTicks  = parseTimeString(humidifierDurationStr);

        if (!estaHumificadorEncendido() && tick - lastHumidifierTick >= intervaloTicks) {
            setEstadoHumificador(true);
            lastHumidifierTick = tick;
            Serial.println("encendido");
        }
        else if (estaHumificadorEncendido() && tick - lastHumidifierTick >= duracionTicks) {
            setEstadoHumificador(false);
            lastHumidifierTick = tick;  // reinicia para el próximo ciclo
            Serial.println("apagado");
        }
}



#endif