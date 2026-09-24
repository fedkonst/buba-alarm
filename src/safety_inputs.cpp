#include "safety_inputs.h"

void initSafetyInputs() {
    pinMode(PIN_BRAKE_SENSE, INPUT);
    pinMode(PIN_KEY_IGN_SENSE, INPUT);
    pinMode(PIN_PARK_SENSE, INPUT_PULLUP); // Замыкание на массу в положении P
}

bool isBrakePressed() {
    // При нажатом тормозе на пине через делитель появляется > 2.0V
    return (analogReadMilliVolts(PIN_BRAKE_SENSE) > 1800);
}

bool isInParkPosition() {
    // Разрешение запуска: сигнал P/N притянут к массе (LOW)
    return (digitalRead(PIN_PARK_SENSE) == LOW);
}

bool isKeyInIgnition() {
    // Ключ повернут в замок (Положение II): на пине через делитель > 1.8V
    return (analogReadMilliVolts(PIN_KEY_IGN_SENSE) > 1800);
}