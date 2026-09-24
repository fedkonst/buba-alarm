#include "ignition.h"
#include "config.h"

#define DEBOUNCE_DELAY_MS 50 // Задержка фильтрации дребезга (50 мс)

// Состояния концевика дверей
static bool debouncedDoorState = false; 
static bool lastRawDoorState = false;
static unsigned long lastDoorDebounceTime = 0;

// Состояния зажигания
static bool debouncedIgnitionState = false;
static bool lastRawIgnitionState = false;
static unsigned long lastIgnitionDebounceTime = 0;

void initSensors() {
    pinMode(PIN_IGNITION_IN, INPUT_PULLUP);
    pinMode(PIN_DOORS_IN, INPUT_PULLUP);
    Serial.println("[IGNITION] Датчики PC817 инициализированы (GPIO 16: IGN, GPIO 17: DOOR)");
}

void updateSensors() {
    unsigned long now = millis();

    // --- 1. Фильтрация концевика дверей ---
    // На вход PC817 при открытии двери подается масса -> транзистор открыт -> LOW
    bool currentRawDoor = (digitalRead(PIN_DOORS_IN) == LOW);

    // Если сырой сигнал изменился (помеха или нажатие)
    if (currentRawDoor != lastRawDoorState) {
        lastDoorDebounceTime = now; // Перезапускаем таймер
        lastRawDoorState = currentRawDoor;
    }

    // Изменение подтверждается только если сигнал стабилен дольше 50 мс
    if ((now - lastDoorDebounceTime) > DEBOUNCE_DELAY_MS) {
        if (currentRawDoor != debouncedDoorState) {
            debouncedDoorState = currentRawDoor;
            if (debouncedDoorState) {
                Serial.println("[SENSOR] Дверь ОТКРЫТА");
            } else {
                Serial.println("[SENSOR] Дверь ЗАКРЫТА");
            }
        }
    }

    // --- 2. Фильтрация сигнала зажигания ---
    bool currentRawIgnition = (digitalRead(PIN_IGNITION_IN) == LOW);

    if (currentRawIgnition != lastRawIgnitionState) {
        lastIgnitionDebounceTime = now;
        lastRawIgnitionState = currentRawIgnition;
    }

    if ((now - lastIgnitionDebounceTime) > DEBOUNCE_DELAY_MS) {
        debouncedIgnitionState = currentRawIgnition;
    }
}

bool isDoorOpen() {
    return debouncedDoorState;
}

bool isIgnitionOn() {
    return debouncedIgnitionState;
}