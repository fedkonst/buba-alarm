#ifndef TACHOMETER_H
#define TACHOMETER_H

#include <Arduino.h>

#define PIN_TACHO_INPUT     18   // Пин с аппаратно поддерживаемыми прерываниями (GPIO 18)
#define ENGINE_RUN_RPM      550  // Порог оборотов, при которых двигатель считается запущенным
#define PULSES_PER_REV      2.5f // Количество импульсов тахометра за 1 оборот (B5254)

void initTachometer();
uint16_t getEngineRPM();
bool isEngineRunning();

#endif // TACHOMETER_H