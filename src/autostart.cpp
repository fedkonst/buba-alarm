#include <Arduino.h>
#include "tachometer.h"

#define PIN_RELAY_IGNITION1 25
#define PIN_RELAY_IGNITION2 26
#define PIN_RELAY_STARTER   27

#define MAX_STARTER_TIME_MS 3500 // Максимальное время работы стартера (3.5 сек)

bool safeEngineStartSequence() {
    Serial.println("[AUTOSTART] Запуск последовательности зажигания...");

    // 1. Включаем Зажигание 1 и 2
    digitalWrite(PIN_RELAY_IGNITION1, HIGH);
    digitalWrite(PIN_RELAY_IGNITION2, HIGH);
    delay(2000); // Ожидание 2 секунды (накачка топлива, инициализация ЭБУ)

    // 2. Отключаем Зажигание 2 (печку/климат) на время работы стартера
    digitalWrite(PIN_RELAY_IGNITION2, LOW);

    Serial.println("[AUTOSTART] Включение стартера...");
    digitalWrite(PIN_RELAY_STARTER, HIGH);

    unsigned long startTimestamp = millis();
    bool engineStarted = false;

    // 3. Цикл прокрутки стартера с непрерывным контролем оборотов
    while (millis() - startTimestamp < MAX_STARTER_TIME_MS) {
        uint16_t rpm = getEngineRPM();
        
        // Как только обороты превысили порог — мгновенно отключаем стартер
        if (rpm >= ENGINE_RUN_RPM) {
            engineStarted = true;
            Serial.printf("[AUTOSTART] Успешный пуск! Обороты: %d RPM\n", rpm);
            break;
        }
        delay(10); // Минимальная задержка опроса
    }

    // 4. МГНОВЕННОЕ отключение стартера
    digitalWrite(PIN_RELAY_STARTER, LOW);

    if (engineStarted) {
        delay(500);
        // Возвращаем питание Зажигания 2 (печка/климат)
        digitalWrite(PIN_RELAY_IGNITION2, HIGH);
        return true;
    } else {
        // Если за 3.5 сек мотор не завелся — обесточиваем зажигание
        digitalWrite(PIN_RELAY_IGNITION1, LOW);
        Serial.println("[AUTOSTART ERROR] Превышено время прокрутки стартера!");
        return false;
    }
}