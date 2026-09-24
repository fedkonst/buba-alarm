#include "tachometer.h"

static volatile unsigned long pulseCount = 0;
static unsigned long lastCalcTime = 0;
static uint16_t currentRPM = 0;

// Функция обработки прерывания (ISR) — вызывается по спадающему фронту (FALLING)
void IRAM_ATTR tachoPulseISR() {
    pulseCount++;
}

void initTachometer() {
    pinMode(PIN_TACHO_INPUT, INPUT_PULLUP);
    // Настраиваем прерывание при подтягивании пина к GND транзистором PC817
    attachInterrupt(digitalPinToInterrupt(PIN_TACHO_INPUT), tachoPulseISR, FALLING);
    lastCalcTime = millis();
}

uint16_t getEngineRPM() {
    unsigned long now = millis();
    unsigned long timeDiff = now - lastCalcTime;

    // Пересчитываем показания каждые 100 мс для быстрой реакции на запуск
    if (timeDiff >= 100) {
        // Атомарно считываем и сбрасываем счетчик импульсов
        noInterrupts();
        unsigned long pulses = pulseCount;
        pulseCount = 0;
        interrupts();

        // Расчет частоты Гц и перевод в об/мин (RPM)
        float freqHz = (pulses * 1000.0f) / timeDiff;
        currentRPM = (uint16_t)((freqHz / PULSES_PER_REV) * 60.0f);

        lastCalcTime = now;
    }

    return currentRPM;
}

bool isEngineRunning() {
    return (getEngineRPM() >= ENGINE_RUN_RPM);
}