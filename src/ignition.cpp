#include "ignition.h"

void initSensors() {
    // Подтягиваем входы к +3.3В через внутренний резистор ESP32.
    // Когда оптопара PC817 срабатывает (транзистор открывается),
    // она подтягивает пин к GND (логический LOW).
    pinMode(IGNITION_PIN, INPUT_PULLUP);
    pinMode(DOOR_PIN, INPUT_PULLUP);
    
    Serial.println("[IGNITION] Датчики PC817 инициализированы (GPIO 16: IGN, GPIO 17: DOOR)");
}

void initIgnition() {
    initSensors();
    Serial.println("[FINGERPRINT] Датчик отпечатков отключен в настройках");
}

// Зажигание ВКЛ (+12В подано -> светодиод PC817 горит -> транзистор открыт -> LOW)
bool isIgnitionOn() {
    return (digitalRead(IGNITION_PIN) == LOW);
}

// Дверь ОТКРЫТА (концевик замкнут на массу -> светодиод PC817 горит -> транзистор открыт -> LOW)
bool isDoorOpen() {
    return (digitalRead(DOOR_PIN) == LOW);
}
bool checkFingerprint() {
    // Serial.println("[FINGERPRINT] Сканер отключен (Bypass Active)");
    return true; 
}