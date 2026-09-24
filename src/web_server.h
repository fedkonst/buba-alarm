#pragma once
#include <Arduino.h>

void initWebServer();
void updateWebTelemetry();

// Геттеры настроек для main.cpp / relays.cpp
bool getSpeedLockEnabled();
bool getUnlockOnStopEnabled();