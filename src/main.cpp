#include <Arduino.h>
#include "measurements.h"
#include "tachometer.h"
#include "safety_inputs.h"
#include "autostart_engine.h"
#include "web_server.h"

// Пины реле зажигания и стартера
#define PIN_RELAY_IGN1       25
#define PIN_RELAY_IGN2       26
#define PIN_RELAY_ST         27

// Сервисные пины (свет, поворотники, замки)
#define PIN_RELAY_HAZARDS    14  // Аварийка / поворотники
#define PIN_RELAY_HEADLIGHT  12  // Ближний свет
#define PIN_RELAY_LOCK       32  // ЦЗ - Закрыть
#define PIN_RELAY_UNLOCK     33  // ЦЗ - Открыть

static bool isSystemArmed = true;
static bool doorsLockedBySpeed = false;
static bool prevEngineRunning = false;

// Импульс на реле ЦЗ (350 мс)
void pulseRelay(uint8_t pin, uint32_t durationMs = 350) {
    digitalWrite(pin, HIGH);
    delay(durationMs);
    digitalWrite(pin, LOW);
}

// Мигание поворотниками
void blinkHazards(int times, int delayMs = 180) {
    for (int i = 0; i < times; i++) {
        digitalWrite(PIN_RELAY_HAZARDS, HIGH);
        delay(delayMs);
        digitalWrite(PIN_RELAY_HAZARDS, LOW);
        if (i < times - 1) delay(delayMs);
    }
}

// Постановка / Снятие с охраны
void setArmedState(bool arm) {
    isSystemArmed = arm;
    blinkHazards(2, 160); // 2 вспышки подтверждения
    pulseRelay(arm ? PIN_RELAY_LOCK : PIN_RELAY_UNLOCK);
    Serial.printf("[SYS] Охрана: %s | ЦЗ: %s\n", arm ? "ВКЛ" : "ВЫКЛ", arm ? "ЗАКРЫТ" : "ОТКРЫТ");
}

bool getSystemArmed() {
    return isSystemArmed;
}

// Запуск двигателя: 3 вспышки -> запуск алгоритма
void triggerEngineStart() {
    Serial.println("[DVS] Старт автозапуска: 3 вспышки аварийки...");
    blinkHazards(3, 200);
    startAutostartSequence(isSystemArmed);
}

// Глушение двигателя
void triggerEngineStop(const char* reason) {
    stopEngine(reason);
    digitalWrite(PIN_RELAY_HEADLIGHT, LOW);
    Serial.println("[DVS] Двигатель остановлен, фары выключены.");
}

// Логика Speed Lock и автоматического света
void processSpeedLock() {
    bool currentRunning = (getAutostartState() == AUTOSTART_RUNNING || 
                           getAutostartState() == AUTOSTART_KEY_TAKEOVER_COMPLETE);
    int currentRpm = getEngineRPM();

    // Запирание в движении
    if (getSpeedLockEnabled() && currentRunning && !doorsLockedBySpeed) {
        if (!isInParkPosition() || currentRpm > 1100) {
            pulseRelay(PIN_RELAY_LOCK);
            doorsLockedBySpeed = true;
            Serial.println("[SpeedLock] Двери заблокированы в движении.");
        }
    }

    // Отпирание при глушении
    if (getUnlockOnStopEnabled() && prevEngineRunning && !currentRunning) {
        pulseRelay(PIN_RELAY_UNLOCK);
        doorsLockedBySpeed = false;
        Serial.println("[SpeedLock] Двери разблокированы после остановки.");
    }

    // Включение света при работающем моторе
    if (currentRunning) {
        digitalWrite(PIN_RELAY_HEADLIGHT, HIGH);
    } else {
        digitalWrite(PIN_RELAY_HEADLIGHT, LOW);
    }

    prevEngineRunning = currentRunning;
}

// Обработка текстовых команд через Serial
void handleSerialCommands() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        cmd.toUpperCase();

        if (cmd == "START") {
            triggerEngineStart();
        } else if (cmd == "STOP") {
            triggerEngineStop("Принудительная остановка по Serial");
        } else if (cmd == "ARM") {
            setArmedState(true);
        } else if (cmd == "DISARM") {
            setArmedState(false);
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);

    // Пины реле
    pinMode(PIN_RELAY_IGN1, OUTPUT);
    pinMode(PIN_RELAY_IGN2, OUTPUT);
    pinMode(PIN_RELAY_ST, OUTPUT);
    pinMode(PIN_RELAY_HAZARDS, OUTPUT);
    pinMode(PIN_RELAY_HEADLIGHT, OUTPUT);
    pinMode(PIN_RELAY_LOCK, OUTPUT);
    pinMode(PIN_RELAY_UNLOCK, OUTPUT);
    
    digitalWrite(PIN_RELAY_IGN1, LOW);
    digitalWrite(PIN_RELAY_IGN2, LOW);
    digitalWrite(PIN_RELAY_ST, LOW);
    digitalWrite(PIN_RELAY_HAZARDS, LOW);
    digitalWrite(PIN_RELAY_HEADLIGHT, LOW);
    digitalWrite(PIN_RELAY_LOCK, LOW);
    digitalWrite(PIN_RELAY_UNLOCK, LOW);

    // Инициализация модулей
    initBatterySensor();
    initTachometer();
    initSafetyInputs();
    initAutostartEngine();
    initWebServer();

    Serial.println("\n=============================================");
    Serial.println("  BUBA ALARM - VOLVO 850 READY               ");
    Serial.println("=============================================");
}

void loop() {
    // 1. Контроль безопасности
    processAutostartSafetyLoop();

    // 2. Логика Speed Lock и света
    processSpeedLock();

    // 3. Serial команды
    handleSerialCommands();

    // 4. Отправка телеметрии в веб раз в 250 мс
    static unsigned long lastWsUpdate = 0;
    if (millis() - lastWsUpdate >= 250) {
        lastWsUpdate = millis();
        updateWebTelemetry();
    }

    // 5. Вывод статуса в Serial раз в 1 сек
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint >= 1000) {
        lastPrint = millis();

        AutostartState st = getAutostartState();
        const char* stateStr = "IDLE";
        if (st == AUTOSTART_RUNNING) {
            stateStr = "RUNNING";
        } else if (st == AUTOSTART_KEY_TAKEOVER_COMPLETE) {
            stateStr = "KEY_TAKEOVER";
        }

        Serial.printf("[STATUS] АКБ: %.2fV | RPM: %d | Охрана: %s | Автозапуск: %s | P: %d | Brake: %d | Key: %d\n",
                      readBatteryVoltage(),
                      getEngineRPM(),
                      isSystemArmed ? "ARMED" : "DISARMED",
                      stateStr,
                      isInParkPosition(),
                      isBrakePressed(),
                      isKeyInIgnition());
    }
}