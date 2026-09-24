#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <Arduino.h>

// Конфигурация пина АЦП
#define PIN_BATTERY_ADC 34

// Параметры делителя напряжения
#define R1_VALUE 100000.0f // 100 кОм
#define R2_VALUE 22000.0f  // 22 кОм
#define VOLTAGE_DIVIDER_RATIO ((R1_VALUE + R2_VALUE) / R2_VALUE)

void initBatterySensor();
float readBatteryVoltage();

#endif // MEASUREMENTS_H