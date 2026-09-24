#ifndef POWER_SAVE_H
#define POWER_SAVE_H

#include <Arduino.h>

void initPowerSave();
void enterLightSleep(uint32_t sleepMs);

#endif // POWER_SAVE_H