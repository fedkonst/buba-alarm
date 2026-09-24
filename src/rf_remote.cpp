#include "rf_remote.h"
#include "ble_tag.h"
#include "config.h"
#include <RCSwitch.h>

#define DEBOUNCE_MS 200          // Задержка между нажатиями (200 мс)
#define MULTI_CLICK_TIMEOUT 600  // Тайм-аут ожидания тройного клика (600 мс)

const unsigned long CODE_LOCK   = 4933471; // Кнопка 1 (Закрыть)
const unsigned long CODE_UNLOCK = 4933468; // Кнопка 2 (Открыть)
const unsigned long CODE_TRUNK  = 4933464; // Кнопка 3 (Багажник)
const unsigned long CODE_SEARCH = 0;       // Кнопка 4 (Поиск / Аварийка)

static RCSwitch mySwitch = RCSwitch();
static unsigned long lastSignalTime = 0;

static bool lightActive = false;
static unsigned long lightStartTime = 0;

// Счётчик нажатий для кнопки LOCK
static uint8_t lockClickCount = 0;
static unsigned long lastLockClickTime = 0;

void courtesy_light_start() {
    digitalWrite(PIN_WELCOME_LIGHT, HIGH);
    lightStartTime = millis();
    lightActive = true;
    Serial.println("[LIGHTS] Вежливый свет ВКЛЮЧЕН (15 сек)");
}

void courtesy_light_loop() {
    if (lightActive && (millis() - lightStartTime >= COURTESY_LIGHT_MS)) {
        digitalWrite(PIN_WELCOME_LIGHT, LOW);
        lightActive = false;
        Serial.println("[LIGHTS] Время истекло -> Вежливый свет ВЫКЛЮЧЕН");
    }
}

void rf_remote_init() {
    pinMode(PIN_WELCOME_LIGHT, OUTPUT);
    digitalWrite(PIN_WELCOME_LIGHT, LOW);
    mySwitch.enableReceive(digitalPinToInterrupt(PIN_RF_RX));
    Serial.printf("[RF] Приемник RXB6 готов на GPIO %d\n", PIN_RF_RX);
}

void rf_remote_loop(SystemState &state) {
    unsigned long now = millis();
    courtesy_light_loop();

    // Обработка таймера одиночного/двойного клика
    if (lockClickCount > 0 && (now - lastLockClickTime > MULTI_CLICK_TIMEOUT)) {
        if (lockClickCount < 3) {
            // Обычная постановка на охрану, если кликов было меньше трех
            Serial.println("[RF] Постановка на охрану с пульта + Вежливый свет");
            state = STATE_ARMED;
            courtesy_light_start();
        }
        lockClickCount = 0; // Сброс счетчика
    }

    if (!mySwitch.available()) {
        return;
    }

    unsigned long receivedValue = mySwitch.getReceivedValue();

    if (receivedValue != 0 && (now - lastSignalTime > DEBOUNCE_MS)) {
        lastSignalTime = now;

        if (receivedValue == CODE_LOCK) {
            lockClickCount++;
            lastLockClickTime = now;
            
            Serial.printf("[RF] Кнопка LOCK нажата (%d/3)\n", lockClickCount);

            if (lockClickCount == 3) {
                Serial.println("[RF] ТРОЙНОЕ НАЖАТИЕ -> Переключение Hands-Free!");
                toggleKeylessMode(); // Переключаем Hands-Free и сохраняем в NVS
                lockClickCount = 0;  // Сброс счетчика
            }
        } 
        else if (receivedValue == CODE_UNLOCK) {
            lockClickCount = 0;
            Serial.println("[RF] Команда: Снятие с охраны");
            state = STATE_DISARMED;
            
            if (lightActive) {
                digitalWrite(PIN_WELCOME_LIGHT, LOW);
                lightActive = false;
            }
            
            setManualUnlock();
        } 
        else if (receivedValue == CODE_TRUNK) {
            lockClickCount = 0;
            Serial.println("[RF] Команда: Открытие багажника");
        }
        else if (receivedValue == CODE_SEARCH && CODE_SEARCH != 0) {
            lockClickCount = 0;
            Serial.println("[RF] Команда: Поиск на парковке");
            triggerHazards(3);
        }
        else {
            Serial.printf("[RF UNKNOWN] Получен новый код: %lu (Длина: %d, Протокол: %d)\n", 
                          receivedValue, 
                          mySwitch.getReceivedBitlength(), 
                          mySwitch.getReceivedProtocol());
        }
    }

    mySwitch.resetAvailable();
}