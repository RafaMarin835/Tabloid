#ifndef NEOPIXELMODULE_H
#define NEOPIXELMODULE_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

static Adafruit_NeoPixel *npStrip = nullptr;
static int npNumPixels = 0;
static int npPin = -1;
static uint8_t npBrillo = 255;

void initNeoPixel(int pin, int numPixels) {
  if (npStrip) { delete npStrip; npStrip = nullptr; }
  npPin = pin;
  npNumPixels = numPixels;
  npStrip = new Adafruit_NeoPixel(npNumPixels, npPin, NEO_GRB + NEO_KHZ800);
  npStrip->begin();
  npStrip->show(); // apagar
  Serial.print("Neopixel iniciado");
}

bool hexToRGB(const String &hex, uint8_t &r, uint8_t &g, uint8_t &b) { //convercion de hex a rgb
  String s = hex;
  s.trim();
  if (s.length() == 0) return false;
  if (s.charAt(0) == '#') s = s.substring(1);
  if (s.length() != 6) return false;
  for (uint8_t i = 0; i < 6; ++i) {
    char c = s.charAt(i);
    bool ok = (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
    if (!ok) return false;
  }
  long val = strtol(s.c_str(), NULL, 16);
  if (val < 0) return false;
  r = (val >> 16) & 0xFF;
  g = (val >> 8) & 0xFF;
  b = val & 0xFF;
  return true;
}

void setNeoColor(uint8_t r, uint8_t g, uint8_t b) { //establecer color desde rgb
  if (!npStrip) return;
  for (int i = 0; i < npNumPixels; i++) {
    npStrip->setPixelColor(i, npStrip->Color(r, g, b));
  }
  npStrip->show();
}

bool setNeoColorHex(const String &hex) { //asignar color desde hexa
  uint8_t r, g, b;
  if (!hexToRGB(hex, r, g, b)) return false;
  setNeoColor(r, g, b);
  return true;
}

String getNeoColorHex() { //obtener el color actual de las led en hexadecimal
  if (!npStrip || npNumPixels == 0) return String("");
  uint32_t color = npStrip->getPixelColor(0);
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  char buf[8 + 1];
  sprintf(buf, "#%02X%02X%02X", r, g, b);
  return String(buf);
}

String getNeoColorHexAt(int index) { //obtener el color de una led especifica
  if (!npStrip || npNumPixels == 0) return String("");
  if (index < 0 || index >= npNumPixels) return String("");
  uint32_t color = npStrip->getPixelColor(index);
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  char buf[8 + 1];
  sprintf(buf, "#%02X%02X%02X", r, g, b);
  return String(buf);
}

void setNeoBrillo(uint8_t b) {
  if (!npStrip) { npBrillo = b; return; }
  npBrillo = b;
  npStrip->setBrightness(npBrillo);
  npStrip->show();
}

#endif
