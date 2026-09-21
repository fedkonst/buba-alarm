#pragma once
#include <Arduino.h>
#include "config.h"

void initTacho();
uint16_t getRPM();
bool isEngineRunning();