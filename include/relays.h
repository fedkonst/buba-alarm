#ifndef RELAYS_H
#define RELAYS_H

#include <Arduino.h>
#include "states.h"

// Длительность штатного импульса ЦЗ (300 мс достаточно для всех актуаторов)
#define RELAY_PULSE_MS        300

// Аварийный предел длительности (защита от залипания): не более 1000 мс
#define RELAY_SAFETY_MAX_MS   1000

// Прототипы основных функций
void initRelays();
void updateRelays(SystemState state);

// Принудительный запуск импульсов (для ручного управления/RF-пульта)
void triggerLockPulse();
void triggerUnlockPulse();
void stopAllRelays();

#endif // RELAYS_H