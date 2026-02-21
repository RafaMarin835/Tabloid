#ifndef BUTTONMODULE_H
#define BUTTONMODULE_H

#include <Arduino.h>

/*---------Lo necesario para trabajar----------
initButton(pin, callback) -> en setup y recibe el pin y el retorno al precionarlo
tickButton()   -> se llama en el loop constantemente

--------------------------------*/
// Manejador de botón con antirrebote
static int botonPin = -1;
static bool botonPrevio = HIGH;
static unsigned long tiempoUltimoCambioMs = 0;
static const unsigned long tiempoAntirreboteMs = 50;
static void (*onPressCallback)() = nullptr;

//llamarse en el stup
void initButton(int pin, void (*onPress)()) {
  botonPin = pin;
  pinMode(botonPin, INPUT_PULLUP);
  botonPrevio = digitalRead(botonPin);
  tiempoUltimoCambioMs = millis();
  onPressCallback = onPress;
  Serial.print("Boton iniciado");
}

// Debe llamarse frecuentemente en loop (no depende de ticks)
void tickButton() {
  if (botonPin < 0) return;
  bool lectura = digitalRead(botonPin);
  if (lectura != botonPrevio) {
    tiempoUltimoCambioMs = millis();
    botonPrevio = lectura;
  }

  if ((millis() - tiempoUltimoCambioMs) > tiempoAntirreboteMs) {
    // pulsación estable detectada (activo en LOW)
    if (lectura == LOW) {
      // Ejecutar callback si está definido
      if (onPressCallback) {
        // Esperar a que suelte en esta misma función como en tu código original
        while (digitalRead(botonPin) == LOW) { delay(1); } // bloqueo corto para esperar liberación
        delay(50); // estabilizar
        onPressCallback();
      }
      // evitar volver a llamar hasta que cambie el estado
      tiempoUltimoCambioMs = millis();
    }
  }
}

#endif
