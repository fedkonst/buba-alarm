#include "states.h"
#include <Arduino.h>

// GPIO пины управления реле ЦЗ
#define RELAY_LOCK_PIN   12 // Пин реле «Закрыть»
#define RELAY_UNLOCK_PIN 13 // Пин реле «Открыть»

static SystemState lastState = STATE_ARMED;

void updateRelays(SystemState state) {
    // Если состояние не изменилось — ничего не делаем (чтобы не щелкать реле постоянным сигналом)
    if (state == lastState) {
        return;
    }

    if (state == STATE_ARMED) {
        Serial.println("[RELAYS] Постановка на охрану -> Импульс ЦЗ 'ЗАКРЫТЬ'");
        digitalWrite(RELAY_LOCK_PIN, HIGH);
        delay(300); // Короткий импульс 300 мс для срабатывания актуатора ЦЗ Volvo
        digitalWrite(RELAY_LOCK_PIN, LOW);
    } 
    else if (state == STATE_DISARMED) {
        Serial.println("[RELAYS] Снятие с охраны -> Импульс ЦЗ 'ОТКРЫТЬ'");
        digitalWrite(RELAY_UNLOCK_PIN, HIGH);
        delay(300); // Короткий импульс 300 мс
        digitalWrite(RELAY_UNLOCK_PIN, LOW);
    }

    lastState = state;
}
