#ifndef BLE_TAG_H
#define BLE_TAG_H

#include "states.h"
#include <Arduino.h>

#define RSSI_NEAR_THRESHOLD  -70
#define RSSI_FAR_THRESHOLD   -88
#define TAG_TIMEOUT_MS      4000

struct KeylessState {
    bool enabled = true;               // Статус активности Hands-Free
    bool tagPresent = false;
    bool handsFreeSuspended = false;
    unsigned long lastTagSeenMs = 0;
};

extern KeylessState g_keyless;

// Основные прототипы
void initBLE();
void initKeylessSystem();
void processBLE(SystemState &state);
void checkKeylessTimeout();
void setManualUnlock();
void triggerHazards(uint8_t count);

// Функции работы с NVS памятью
void loadKeylessSettings();
void saveKeylessSettings(bool enabled);
void toggleKeylessMode();

#endif // BLE_TAG_H