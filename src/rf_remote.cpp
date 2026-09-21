#include "rf_remote.h"
#include "ble_tag.h"
#include <RCSwitch.h>

#define DEBOUNCE_MS 350

// Уникальные коды кнопок брелока T102
const unsigned long CODE_LOCK   = 4933471; // Кнопка K1 (Закрыть)
const unsigned long CODE_UNLOCK = 4933468; // Кнопка K2 (Открыть)
const unsigned long CODE_TRUNK  = 4933464; // Кнопка K3 (Багажник)

static RCSwitch mySwitch = RCSwitch();
static unsigned long lastSignalTime = 0;

static bool lightActive = false;
static unsigned long lightStartTime = 0;

void courtesy_light_start() {
    digitalWrite(LIGHTS_RELAY_PIN, HIGH);
    lightStartTime = millis();
    lightActive = true;
    Serial.println("[LIGHTS] Вежливый свет ВКЛЮЧЕН (15 сек)");
}

void courtesy_light_loop() {
    if (lightActive && (millis() - lightStartTime >= COURTESY_LIGHT_MS)) {
        digitalWrite(LIGHTS_RELAY_PIN, LOW);
        lightActive = false;
        Serial.println("[LIGHTS] Время истекло -> Вежливый свет ВЫКЛЮЧЕН");
    }
}

void rf_remote_init() {
    pinMode(LIGHTS_RELAY_PIN, OUTPUT);
    digitalWrite(LIGHTS_RELAY_PIN, LOW);

    mySwitch.enableReceive(digitalPinToInterrupt(RF_RX_PIN));
    Serial.println("[RF] Приемник RXB6 готов на GPIO " + String(RF_RX_PIN));
}

void rf_remote_loop(SystemState &state) {
    courtesy_light_loop();

    if (!mySwitch.available()) {
        return;
    }

    unsigned long receivedValue = mySwitch.getReceivedValue();

    if (receivedValue != 0 && (millis() - lastSignalTime > DEBOUNCE_MS)) {
        lastSignalTime = millis();

        if (receivedValue == CODE_LOCK) {
            Serial.println("[RF] Постановка на охрану + Вежливый свет");
            state = STATE_ARMED;
            courtesy_light_start();
        } 
        else if (receivedValue == CODE_UNLOCK) {
            Serial.println("[RF] Снятие с охраны с пульта");
            state = STATE_DISARMED;
            
            // Если горит свет — гасим сразу при снятии
            if (lightActive) {
                digitalWrite(LIGHTS_RELAY_PIN, LOW);
                lightActive = false;
            }
            
            // Активируем окно ожидания 30 сек, чтобы BLE не запечатал машину
            setManualUnlock();
        } 
        else if (receivedValue == CODE_TRUNK) {
            Serial.println("[RF] Команда: Открытие багажника");
        }
    }

    mySwitch.resetAvailable();
}
