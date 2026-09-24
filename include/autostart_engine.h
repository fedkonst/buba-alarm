#ifndef AUTOSTART_ENGINE_H
#define AUTOSTART_ENGINE_H

#include <Arduino.h>

enum AutostartState {
    AUTOSTART_IDLE,
    AUTOSTART_RUNNING,
    AUTOSTART_KEY_TAKEOVER_COMPLETE,
    AUTOSTART_ERROR
};

void initAutostartEngine();
bool startAutostartSequence(bool isSystemArmed);
void processAutostartSafetyLoop();
void stopEngine(const char* reason);

AutostartState getAutostartState();

#endif // AUTOSTART_ENGINE_H