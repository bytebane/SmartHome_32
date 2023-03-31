#ifndef MY_INDICATORS_H
#define MY_INDICATORS_H
#include "Arduino.h"

#define PIN_R 21
#define PIN_G 23
#define PIN_Y 22

void setupStartLED();
void setupEndLED();
void wifiConnectedLED();
void dbConnectedLED();
void wifiErrorLED();
void dbErrorLED();

#endif