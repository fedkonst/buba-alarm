#include "measurements.h"

// Переменная для экспоненциального сглаживания (EMA-фильтр)
static float smoothedVoltage = 0.0f;

void initBatterySensor() {
    // Разрешение АЦП 12 бит (0..4095)
    analogReadResolution(12);
    
    // Ослабление 11 dB позволяет измерять диапазон 0..3.3V
    analogSetPinAttenuation(PIN_BATTERY_ADC, ADC_11db);

    // Первичная инициализация сглаженного значения
    float rawVolts = analogReadMilliVolts(PIN_BATTERY_ADC) / 1000.0f;
    smoothedVoltage = rawVolts * VOLTAGE_DIVIDER_RATIO;
}

float readBatteryVoltage() {
    // 1. Усреднение по 30 измерениям для подавления шумов
    float sumVolts = 0.0f;
    for (int i = 0; i < 30; i++) {
        sumVolts += analogReadMilliVolts(PIN_BATTERY_ADC);
        delayMicroseconds(100);
    }
    
    float currentVolts = (sumVolts / 30.0f / 1000.0f) * VOLTAGE_DIVIDER_RATIO;
    
    // 2. Экспоненциальный фильтр (EMA) для плавных показаний без скачков
    // Альфа = 0.1 (чем меньше значение, тем выше сглаживание)
    smoothedVoltage = (0.10f * currentVolts) + (0.90f * smoothedVoltage);

    return smoothedVoltage;
}