#pragma once

// --- Входы (Опторазвязка PC817: при +12V на входе выдает LOW) ---
#define PIN_IGNITION_IN    16  // Сигнал зажигания (+12V)
#define PIN_DOORS_IN       17  // Концевики дверей (Минус/Масса)
#define PIN_BRAKE_IN       32  // Педаль тормоза
#define PIN_TACHO_IN       33  // Тахометр
#define PIN_RF_RX           4  // Приемник RXB6 433 МГц

// --- Выходы (Модули реле и ключи) ---
#define PIN_RELAY_LOCK     12  // Реле ЦЗ "Закрыть"
#define PIN_RELAY_UNLOCK   13  // Реле ЦЗ "Открыть"
#define PIN_RELAY_ACC      26  // Реле ACC
#define PIN_RELAY_IGN      27  // Реле Зажигания
#define PIN_RELAY_STARTER  14  // Реле Стартера
#define PIN_HAZARDS        25  // Выход на аварийку
#define PIN_WELCOME_LIGHT   2  // Выход на вежливый свет

// Активный уровень реле
#define RELAY_ON           LOW
#define RELAY_OFF          HIGH

// --- BLE Настройки ---
#define TARGET_TAG_MAC "48:87:2d:9e:1e:a4"