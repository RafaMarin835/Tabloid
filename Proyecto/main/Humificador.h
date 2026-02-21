#ifndef HUMIFICADORMODULE_H
#define HUMIFICADORMODULE_H

#include <Arduino.h>

// Pin de control
static int humificadorPin = -1;
static int humificadorPinON = -1;


// Pulso
static const unsigned long duracionPulso = 2;       // duración de cada HIGH
static const unsigned long pausaEntrePulsos = 2;    // pausa entre pulsos

// Estado interno
static int etapaPulso = 0;
static bool solicitudPendiente = false;
static bool estadoHumificador = false;              // false = apagado, true = encendido
static bool nuevoEstado = false;
static unsigned long tiempoReferencia = 0;

// Inicializa el módulo
void initHumificador(int pin, int pinON) {
  humificadorPin = pin;
  humificadorPinON = pinON;
  pinMode(humificadorPin, OUTPUT);
  pinMode(humificadorPinON, OUTPUT);
  digitalWrite(humificadorPin, LOW);
  digitalWrite(humificadorPinON, LOW);
  estadoHumificador = false;
}

// Devuelve el estado actual
bool estaHumificadorEncendido() {
  return estadoHumificador;
}

void EncenderApagar(bool opc){
  if (opc){
    digitalWrite(humificadorPinON, HIGH);
    delay(200);
  }
  else{
    digitalWrite(humificadorPinON, LOW);
  }
}

// Solicita encendido o apagado (solo si hay cambio)
void setEstadoHumificador(bool encender) {
  EncenderApagar(true);
  if (!solicitudPendiente && encender != estadoHumificador) {
    solicitudPendiente = true;
    nuevoEstado = encender;
  }
  else if(!encender) {
    EncenderApagar(false);
    estadoHumificador = false;
  }
}

// Llamada periódica desde loop
void actualizarHumificador(unsigned long tiempoReferencia, String estado) {
  if (estado == "OFF") EncenderApagar(false);

  unsigned long ahora = millis();

  if (!solicitudPendiente) return;

  switch (etapaPulso) {
    case 0:
      digitalWrite(humificadorPin, HIGH);
      tiempoReferencia = ahora;
      etapaPulso = 1;
      break;

    case 1:
      if (ahora - tiempoReferencia >= duracionPulso) {
        digitalWrite(humificadorPin, LOW);
        tiempoReferencia = ahora;
        etapaPulso = 2;
      }
      break;

    case 2:
      if (ahora - tiempoReferencia >= pausaEntrePulsos) {
        digitalWrite(humificadorPin, HIGH);
        tiempoReferencia = ahora;
        etapaPulso = 3;
      }
      break;

    case 3:
      if (ahora - tiempoReferencia >= duracionPulso) {
        digitalWrite(humificadorPin, LOW);
        estadoHumificador = nuevoEstado;
        solicitudPendiente = false;
        etapaPulso = 0;
      }
      break;
  }
}

#endif