#include "power_save.h"
#include "config.h"
#include "esp_sleep.h"
#include "driver/gpio.h"

void initPowerSave() {
    // 1. Включаем возможность пробуждения по изменению уровней на GPIO
    // Сигналы с PC817 при срабатывании проседают в LOW (0V)
    gpio_wakeup_enable((gpio_num_t)PIN_IGNITION_IN, GPIO_INTR_LOW_LEVEL);
    gpio_wakeup_enable((gpio_num_t)PIN_DOORS_IN, GPIO_INTR_LOW_LEVEL);

    // 2. Пробуждение по сигналу с радиоприемника 433 МГц (RXB6)
    // Приемник при поступлении радиопакета выдает импульсы
    gpio_wakeup_enable((gpio_num_t)PIN_RF_RX, GPIO_INTR_LOW_LEVEL);

    // 3. Регистрируем источник пробуждения GPIO для Light Sleep
    esp_sleep_enable_gpio_wakeup();

    Serial.println("[POWER] Источники пробуждения GPIO (Зажигание, Двери, RF-RX) настроены");
}

void enterLightSleep(uint32_t sleepMs) {
    // 1. Настраиваем таймерное пробуждение (для циклического BLE сканирования)
    esp_sleep_enable_timer_wakeup((uint64_t)sleepMs * 1000);

    // 2. Уходим в Light Sleep
    // Выполнение кода останавливается на этой строчке
    esp_light_sleep_start();

    // 3. Код продолжается сразу отсюда при наступлении любого из событий (Таймер / GPIO)
}