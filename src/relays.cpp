#include "relays.h"
#include "config.h"

// Активный уровень реле (по умолчанию HIGH. Если модуль реле сработает от LOW — переопределите в config.h)
#ifndef RELAY_ACTIVE_LEVEL
#define RELAY_ACTIVE_LEVEL HIGH
#endif

#define RELAY_INACTIVE_LEVEL (!RELAY_ACTIVE_LEVEL)

static SystemState lastState = STATE_ARMED;

static bool isLockActive = false;
static bool isUnlockActive = false;
static unsigned long pulseStartMs = 0;

void initRelays() {
    // 1. Сначала принудительно гасим пины, избегая "подтягивающего" щелчка реле при перезагрузке ESP32
    digitalWrite(PIN_RELAY_LOCK, RELAY_INACTIVE_LEVEL);
    digitalWrite(PIN_RELAY_UNLOCK, RELAY_INACTIVE_LEVEL);

    // 2. Устанавливаем режим выходов
    pinMode(PIN_RELAY_LOCK, OUTPUT);
    pinMode(PIN_RELAY_UNLOCK, OUTPUT);

    // 3. Повторная страховка
    stopAllRelays();

    Serial.println("[RELAYS] Модуль управления реле ЦЗ готов (Защита от залипания АКТИВНА)");
}

void stopAllRelays() {
    digitalWrite(PIN_RELAY_LOCK, RELAY_INACTIVE_LEVEL);
    digitalWrite(PIN_RELAY_UNLOCK, RELAY_INACTIVE_LEVEL);
    isLockActive = false;
    isUnlockActive = false;
}

void triggerLockPulse() {
    stopAllRelays(); // Взаимная блокировка (Interlock)

    digitalWrite(PIN_RELAY_LOCK, RELAY_ACTIVE_LEVEL);
    isLockActive = true;
    pulseStartMs = millis();
    Serial.println("[RELAYS] Подача импульса: ЗАКРЫТЬ ЦЗ");
}

void triggerUnlockPulse() {
    stopAllRelays(); // Взаимная блокировка (Interlock)

    digitalWrite(PIN_RELAY_UNLOCK, RELAY_ACTIVE_LEVEL);
    isUnlockActive = true;
    pulseStartMs = millis();
    Serial.println("[RELAYS] Подача импульса: ОТКРЫТЬ ЦЗ");
}

void updateRelays(SystemState state) {
    unsigned long now = millis();

    // --- 1. Отслеживание смены состояния системы ---
    if (state != lastState) {
        if (state == STATE_ARMED) {
            triggerLockPulse();
        } else if (state == STATE_DISARMED) {
            triggerUnlockPulse();
        }
        lastState = state;
    }

    // --- 2. Контроль длительности активного импульса ---
    if (isLockActive || isUnlockActive) {
        unsigned long elapsed = now - pulseStartMs;

        // Штатное отключение реле через 300 мс
        if (elapsed >= RELAY_PULSE_MS) {
            stopAllRelays();
            Serial.println("[RELAYS] Импульс ЦЗ штатно завершен");
        }
        // Аварийный отклик: прерывание при превышении 1000 мс
        else if (elapsed >= RELAY_SAFETY_MAX_MS) {
            stopAllRelays();
            Serial.println("[CRITICAL] [RELAYS] АВАРИЙНОЕ ОТКЛЮЧЕНИЕ! Превышен предел времени работы реле!");
        }
    } 
    // --- 3. Страховочный фильтр в состоянии простоя ---
    else {
        // Если флаги выключены, но физический уровень на GPIO остался активным (например, аппаратный сбой пина)
        if (digitalRead(PIN_RELAY_LOCK) == RELAY_ACTIVE_LEVEL || 
            digitalRead(PIN_RELAY_UNLOCK) == RELAY_ACTIVE_LEVEL) {
            stopAllRelays();
            Serial.println("[WARNING] [RELAYS] Обнаружен случайный сигнал на GPIO реле. Питание снято.");
        }
    }
}