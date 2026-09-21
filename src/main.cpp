#include <Arduino.h>
#include "config.h"
#include "states.h"
#include "ignition.h"
#include "tacho.h"
#include "ble_tag.h"
#include "rf_remote.h"

// Глобальное состояние системы (по умолчанию при включении — на охране)
SystemState currentState = STATE_ARMED;

void setup() {
    // 1. Инициализация отладочного порту
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n==================================================");
    Serial.println("   VOLVO 850 - ESP32 AUTOMOTIVE SECURITY SYSTEM   ");
    Serial.println("==================================================");

    // 2. Инициализация радиоприемника RXB6 (433 МГц) и вежливого света
    rf_remote_init();

    // 3. Инициализация оптопар PC817 (Зажигание и концевики дверей)
    initSensors(); 

    // 4. Инициализация считывания оборотов двигателя (Тахометр)
    initTacho();

    // 5. Инициализация BLE модуля (NimBLE сканер)
    initBLE();

    // 6. Загрузка настроек режима Hands-Free из энергонезависимой памяти (NVS)
    initKeylessSystem();

    Serial.println("[SYSTEM] Все модули успешно инициализированы.");
    Serial.println("[SYSTEM] Готов к работе! Ожидание сигналов с пульта T102 или BLE-метки...\n");
}

void loop() {
    // 1. Прием сигналов 433 МГц с пульта T102 + таймер вежливого света (15 сек)
    rf_remote_loop(currentState);

    // 2. Логика бесключевого доступа BLE (метрика RSSI, оверрайд 30 сек)
    processBLE(currentState);

    // 3. Проверка таймаута потери BLE-метки (автоматическое закрытие при уходе)
    checkKeylessTimeout();

    // 4. Обновление состояния физических реле (ЦЗ, зажигание, блокировки)
    updateRelays(currentState);

    // Небольшая задержка цикла для стабильности и снижения нагрузки на процессор
    delay(10);
}
