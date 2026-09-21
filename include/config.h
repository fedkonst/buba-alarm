#pragma once

// --- Входы (Опторазвязка PC817: при 12V на входе выдает LOW на ESP32) ---
#define PIN_DOORS_IN       34  // Вход OUT1 (Двери)
#define PIN_BRAKE_IN       32  // Вход OUT2 (Педаль тормоза)
#define PIN_TACHO_IN       33  // Вход OUT3 (Тахометр)
#define PIN_BTN_START      25  // Вход OUT4 (Кнопка Старт/Стоп)

// --- Выходы (Модуль реле) ---
#define PIN_RELAY_ACC      26  // Реле 1 (ACC)
#define PIN_RELAY_IGN      27  // Реле 2 (Зажигание)
#define PIN_RELAY_STARTER  14  // Реле 3 (Стартер)
#define PIN_RELAY_LOCK     12  // Реле 4 (Центральный замок)

// Активный уровень реле (LOW для большинства стандартных плат реле)
#define RELAY_ON           LOW
#define RELAY_OFF          HIGH

// --- Настройки BLE-метки ---
#define TARGET_TAG_MAC "48:87:2D:9E:1E:A4" // Вставьте сюда MAC-адрес вашей метки (в нижнем регистре)
#define RSSI_THRESHOLD -85                 // Порог срабатывания по дальности (в dBm)
