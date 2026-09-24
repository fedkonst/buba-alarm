#ifndef SAFETY_INPUTS_H
#define SAFETY_INPUTS_H

#include <Arduino.h>

#define PIN_BRAKE_SENSE     35 // +12V при нажатии тормоза (через делитель)
#define PIN_KEY_IGN_SENSE   33 // +12V от замка зажигания в положении II (через делитель)
#define PIN_PARK_SENSE      32 // Сигнал положения P/N АКПП (GND при P/N)

void initSafetyInputs();
bool isBrakePressed();
bool isInParkPosition();
bool isKeyInIgnition();

#endif // SAFETY_INPUTS_H