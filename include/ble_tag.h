#ifndef BLE_TAG_H
#define BLE_TAG_H

#include "states.h"

#define RSSI_NEAR_THRESHOLD  -70
#define RSSI_FAR_THRESHOLD   -88
#define TAG_TIMEOUT_MS      4000
#define RF_UNLOCK_GRACE_PERIOD_MS 30000

struct KeylessState {
    bool enabled = true;
    bool tagPresent = false;
    bool handsFreeSuspended = false;
    unsigned long lastTagSeenMs = 0;
};

extern KeylessState g_keyless;

// Прототипы всех функций модуля BLE
void initBLE();
void initKeylessSystem();
void processBLE(SystemState &state);
void checkKeylessTimeout();
void processBLETag(int currentRssi, bool isButtonPressed);
void setManualUnlock();
void handleManualLock();
void handleManualUnlock();
void toggleKeylessMode();

#endif // BLE_TAG_H
