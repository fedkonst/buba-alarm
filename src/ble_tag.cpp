#include "ble_tag.h"
#include "config.h"
#include "ignition.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <Preferences.h>

#define RSSI_WELCOME_LEVEL -75   // Подход к авто (включение вежливого света)
#define RSSI_UNLOCK_LEVEL  -65   // Открытие ЦЗ
#define RSSI_LOCK_LEVEL    -82   // Закрытие ЦЗ
#define COOLDOWN_MS        10000 // Задержка повторного открытия (10 сек)

KeylessState g_keyless;
static Preferences preferences;

static int lastRssi = -100;
static bool tagPresent = false;
static unsigned long lastTagSeenTime = 0;

static unsigned long lastLockedTime = 0;      
static bool lastIgnitionState = false;        
static bool isWelcomeLightActive = false;    

// --- Работа с NVS (Энергонезависимой памятью) ---

void loadKeylessSettings() {
    preferences.begin("keyless", true); // Только чтение
    g_keyless.enabled = preferences.getBool("enabled", true);
    preferences.end();
    Serial.printf("[NVS] Hands-Free загружен из памяти: %s\n", g_keyless.enabled ? "ВКЛ" : "ВЫКЛ");
}

void saveKeylessSettings(bool enabled) {
    preferences.begin("keyless", false); // Запись
    preferences.putBool("enabled", enabled);
    preferences.end();
    g_keyless.enabled = enabled;
    Serial.printf("[NVS] Сохранено новое состояние Hands-Free: %s\n", enabled ? "ВКЛ" : "ВЫКЛ");
}

void toggleKeylessMode() {
    saveKeylessSettings(!g_keyless.enabled);
    
    // Индикация аварийкой: 3 раза — ВКЛ, 1 раз — ВЫКЛ
    triggerHazards(g_keyless.enabled ? 3 : 1);
}

void initKeylessSystem() {
    loadKeylessSettings();
    initBLE();
}

void checkKeylessTimeout() {
    // Таймаут обрабатывается в processBLE
}

void setManualUnlock() {
    lastLockedTime = millis();
    Serial.println("[BLE] Ручное открытие с RF-пульта");
}

void triggerHazards(uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        digitalWrite(PIN_HAZARDS, HIGH);
        delay(150);
        digitalWrite(PIN_HAZARDS, LOW);
        if (i < count - 1) delay(150);
    }
}

void setWelcomeLight(bool enable) {
    digitalWrite(PIN_WELCOME_LIGHT, enable ? HIGH : LOW);
    isWelcomeLightActive = enable;
}

class AdvertisedDeviceCallbacks: public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        if (advertisedDevice->getAddress().toString() == TARGET_TAG_MAC) {
            lastRssi = advertisedDevice->getRSSI();
            lastTagSeenTime = millis();
            tagPresent = true;
        }
    }
};

void initBLE() {
    g_keyless.handsFreeSuspended = false;
    g_keyless.lastTagSeenMs = 0;

    pinMode(PIN_HAZARDS, OUTPUT);
    pinMode(PIN_WELCOME_LIGHT, OUTPUT);
    digitalWrite(PIN_HAZARDS, LOW);
    digitalWrite(PIN_WELCOME_LIGHT, LOW);

    NimBLEDevice::init("Volvo850_Security");
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks(), true);
    pScan->setActiveScan(true);
    pScan->setInterval(100);
    pScan->setWindow(99);

    Serial.println("[SYSTEM] BLE модуль запущен");
}

void processBLE(SystemState &state) {
    unsigned long now = millis();
    bool ignitionOn = isIgnitionOn();

    // 1. Контроль зажигания (работает всегда)
    if (ignitionOn != lastIgnitionState) {
        delay(50);
        lastIgnitionState = ignitionOn;

        if (ignitionOn) {
            Serial.println("[IGNITION] Зажигание ВКЛ -> Закрываем ЦЗ");
            state = STATE_ARMED;
            setWelcomeLight(false);
            return;
        } else {
            Serial.println("[IGNITION] Зажигание ВЫКЛ -> Открываем ЦЗ");
            state = STATE_DISARMED;
            triggerHazards(2);
            return;
        }
    }

    if (ignitionOn) {
        return;
    }

    // 2. Если Hands-Free выключен с пульта — пропускаем радиосканирование
    if (!g_keyless.enabled) {
        return;
    }

    // Сканирование эфира (1 секунда)
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->start(1, false);

    if (now - lastTagSeenTime > TAG_TIMEOUT_MS) {
        tagPresent = false;
        lastRssi = -100;
    }

    // 3. Вежливый свет
    if (tagPresent && lastRssi >= RSSI_WELCOME_LEVEL && state == STATE_ARMED) {
        if (!isWelcomeLightActive) {
            Serial.println("[LIGHT] Приближение -> Включение вежливого света");
            setWelcomeLight(true);
        }
    } else if (!tagPresent || lastRssi < RSSI_WELCOME_LEVEL) {
        if (isWelcomeLightActive) {
            setWelcomeLight(false);
        }
    }

    // 4. Логика Hands-Free
    if (state == STATE_ARMED && tagPresent && lastRssi >= RSSI_UNLOCK_LEVEL) {
        if (now - lastLockedTime < COOLDOWN_MS) {
            return;
        }

        state = STATE_DISARMED;
        setWelcomeLight(false);
        triggerHazards(2);
        Serial.printf("[KEYLESS] Снятие с охраны (RSSI: %d)\n", lastRssi);
    } 
    else if (state == STATE_DISARMED && (!tagPresent || lastRssi <= RSSI_LOCK_LEVEL)) {
        state = STATE_ARMED;
        lastLockedTime = now;
        setWelcomeLight(false);
        triggerHazards(1);
        Serial.printf("[KEYLESS] Постановка на охрану (RSSI: %d)\n", lastRssi);
    }
}