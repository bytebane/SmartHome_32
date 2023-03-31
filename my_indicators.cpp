#include "my_indicators.h"

void setupStartLED() {
  digitalWrite(PIN_R, HIGH);
  digitalWrite(PIN_G, HIGH);
  digitalWrite(PIN_Y, HIGH);
}

void setupEndLED() {
  digitalWrite(PIN_R, LOW);
  digitalWrite(PIN_G, LOW);
  digitalWrite(PIN_Y, LOW);
}

void wifiConnectedLED() {
  digitalWrite(PIN_R, LOW);
  digitalWrite(PIN_Y, HIGH);
  // digitalWrite(PIN_G, LOW);
}

void dbConnectedLED() {
  // digitalWrite(PIN_R, LOW);
  // digitalWrite(PIN_Y, HIGH);
  digitalWrite(PIN_G, HIGH);
}

void wifiErrorLED() {
  digitalWrite(PIN_R, HIGH);
  digitalWrite(PIN_G, LOW);
  digitalWrite(PIN_Y, LOW);
}

void dbErrorLED() {
  digitalWrite(PIN_G, LOW);
  digitalWrite(PIN_R, HIGH);
  // digitalWrite(PIN_Y, LOW);
}