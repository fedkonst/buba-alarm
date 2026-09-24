#ifndef IGNITION_H
#define IGNITION_H

#include <Arduino.h>

// Назначение GPIO для оптопар PC817
#define IGNITION_PIN  16  // Сигнал +12В Зажигания (через PC817)
#define DOOR_PIN      17  // Сигнал концевика двери (минус через PC817)

// Функции инициализации
void initSensors();
void initIgnition();
void initSensors();
void updateSensors(); // Функция опроса и фильтрации в loop()

// Функции считывания текущего состояния
bool isIgnitionOn();
bool isDoorOpen();
bool isIgnitionOn();  // Возвращает отфильтрованное состояние зажигания
bool isDoorOpen();    // Возвращает отфильтрованное состояние дверей

#endif // IGNITION_H
