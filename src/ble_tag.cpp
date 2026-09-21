#include "ble_tag.h"
#include <Arduino.h>
#include <NimBLEDevice.h>

static std::string TARGET_TAG_MAC = "48:87:2d:9e:1e:a4"; 

// Назначение GPIO выводов
#define PIN_IGNITION      34  // Вход зажигания (+12V через делитель)
#define PIN_HAZARDS       26  // Выход на аварийку/поворотники
#define PIN_WELCOME_LIGHT 25  // Выход на вежливый свет/подсветку

// Пороги RSSI и таймауты
#define RSSI_WELCOME_LEVEL -75  // Подход к авто (включение света)
#define RSSI_UNLOCK_LEVEL  -65  // Открытие ЦЗ
#define RSSI_LOCK_LEVEL    -82  // Закрытие ЦЗ
#define COOLDOWN_MS        10000 // Задержка повторного открытия (10 сек)

KeylessState g_keyless;

static int lastRssi = -100;
static bool tagPresent = false;
static unsigned long lastTagSeenTime = 0;

// Таймеры и флаги функций
static unsigned long lastLockedTime = 0;      // Задержка повторного открытия
static bool lastIgnitionState = false;       // Состояние зажигания
static bool isWelcomeLightActive = false;   // Флаг вежливого света

// --- НЕДОСТАЮЩИЕ ФУНКЦИИ ДЛЯ СБОРКИ ---

void initKeylessSystem() {
    initBLE();
}

void checkKeylessTimeout() {
    // Таймаут утери метки обрабатывается внутри processBLE
}

void setManualUnlock() {
    lastLockedTime = millis(); // Сбрасываем задержку при ручном открытии с пульта
    Serial.println("[BLE] Ручное открытие с RF-пульта");
}

// ----------------------------------------

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
    g_keyless.enabled = true;
    g_keyless.handsFreeSuspended = false;
    g_keyless.lastTagSeenMs = 0;

    pinMode(PIN_IGNITION, INPUT);
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
    pScan->start(0, nullptr, false);

    Serial.println("[SYSTEM] Подсистемы безопасности и BLE запущены");
}

void processBLE(SystemState &state) {
    unsigned long now = millis();
    bool ignitionOn = (digitalRead(PIN_IGNITION) == HIGH);

    // 1 & 2. Защита в движении и автозапирание по зажиганию
    if (ignitionOn != lastIgnitionState) {
        delay(50);
        lastIgnitionState = ignitionOn;

        if (ignitionOn) {
            Serial.println("[IGNITION] Зажигание ВКЛ -> Закрываем ЦЗ, отключаем Hands-Free");
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

    if (now - lastTagSeenTime > TAG_TIMEOUT_MS) {
        tagPresent = false;
        lastRssi = -100;
    }

    // 4. Вежливый свет
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

    // 3 & 5. Логика снятия / постановки
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