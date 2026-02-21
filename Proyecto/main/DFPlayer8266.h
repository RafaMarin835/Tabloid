#ifndef DFPLAYER_8266_H
#define DFPLAYER_8266_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Conexión recomendada para ESP8266 con SoftwareSerial:
//  DFPlayer TX -> ESP8266 D6 (GPIO12)
//  DFPlayer RX -> ESP8266 D7 (GPIO13) -> usar divisor si DFPlayer espera 5V en RX
//  VCC -> 5V, GND -> GND

namespace DF8266 {
  static SoftwareSerial dfSerial(13, 12);  // RX, TX (ESP8266 D6=GPIO12, D7=GPIO13)
  static DFRobotDFPlayerMini player;
  static bool initialized = false;

  // Inicializar (llamar en setup). debugSerial opcional (por defecto Serial)
  static bool begin(Stream *debugSerial = &Serial, uint32_t baud = 9600) {
    debugSerial->println(F("DFPlayer init..."));
    dfSerial.begin(baud);
    delay(200);
    if (!player.begin(dfSerial)) {
      debugSerial->println(F("DFPlayer begin failed"));
      initialized = false;
      return false;
    }
    initialized = true;
    player.volume(20); // volumen por defecto 0..30
    debugSerial->println(F("DFPlayer listo"));
    return true;
  }

  static bool isReady() { return initialized; }

  // Controles mínimos
  static void play(uint16_t idx) { if (initialized) player.play(idx); }
  static void playFolder(uint8_t folder, uint8_t file) { if (initialized) player.playFolder(folder, file); }
  static void next() { if (initialized) player.next(); }
  static void previous() { if (initialized) player.previous(); }
  static void pause() { if (initialized) player.pause(); }
  static void resume() { if (initialized) player.start(); }
  static void stop() { if (initialized) player.stop(); }
  static void volume(uint8_t v) { if (initialized) player.volume(constrain(v, 0, 30)); }

  // Procesar eventos simples; llamar periódicamente en loop()
  static void loop(Stream *debugSerial = &Serial) {
    if (!initialized || !player.available()) return;
    int type = player.readType();
    int value = player.read();
    switch (type) {
      case DFPlayerPlayFinished:
        debugSerial->print(F("FINISHED: "));
        debugSerial->println(value);
        break;
      case DFPlayerError:
        debugSerial->print(F("ERROR: "));
        debugSerial->println(value);
        break;
      case DFPlayerCardInserted:
        debugSerial->println(F("SD inserted"));
        break;
      case DFPlayerCardRemoved:
        debugSerial->println(F("SD removed"));
        break;
      default:
        debugSerial->print(F("EVT "));
        debugSerial->print(type);
        debugSerial->print(F(" "));
        debugSerial->println(value);
        break;
    }
  }
}

#endif // DFPLAYER_8266_H