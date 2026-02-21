#ifndef SENSORMODULE_H
#define SENSORMODULE_H

#include <Arduino.h>

// HC-SR04
static int sensorTrig = -1;
static int sensorEcho = -1;
static unsigned long sensorTimerTicks = 0; // contaje de ticks (1 tick = 100 ms)
static float sensorDistanciaCm = 0.0;

// Inicializa pines del sensor (llamar en setup)
void initSensor(int trigPin, int echoPin) {
  sensorTrig = trigPin;
  sensorEcho = echoPin;
  pinMode(sensorTrig, OUTPUT);
  pinMode(sensorEcho, INPUT);
  digitalWrite(sensorTrig, LOW);
  sensorTimerTicks = 0;
  sensorDistanciaCm = 0.0;
  Serial.print("Sensor iniciado");
}

// Llamar periódicamente (por tick) para gestionar cuándo tomar lectura.
// Si prefieres lecturas en loop, puedes llamar a tickSensor(ticks) cada tick.
void tickSensor(unsigned long ticks) {
  // tick interno en ticks
  if (ticks == 0) return;
  if (ticks == sensorTimerTicks) return;
  // sensorTimerTicks no es estrictamente necesario aquí; simplemente hacemos lectura cada llamada
  // Hacemos la medición (no bloqueante excesiva; usa timeout para pulseIn)
  digitalWrite(sensorTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(sensorTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensorTrig, LOW);
  unsigned long duracion = pulseIn(sensorEcho, HIGH, 30000); // timeout 30 ms
  if (duracion > 0) sensorDistanciaCm = duracion * 0.034 / 2.0;
  else sensorDistanciaCm = 999.0; // valor de error
}

// Obtener distancia en cm
float getDistanciaCm() {
  return sensorDistanciaCm;
}

#endif
