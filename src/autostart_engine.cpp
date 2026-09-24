#include "autostart_engine.h"
#include "safety_inputs.h"
#include "tachometer.h"
#include "measurements.h"

#define PIN_RELAY_IGN1  25
#define PIN_RELAY_IGN2  26
#define PIN_RELAY_ST    27

#define WARMUP_TIMEOUT_MS (15 * 60 * 1000) // 15 минут прогрева

static AutostartState currentState = AUTOSTART_IDLE;
static unsigned long engineStartTimestamp = 0;

// Нарастающее время прокрутки стартера для 3 попыток (мс)
static const uint16_t starterAttemptDurations[] = {1500, 2500, 3500};

void initAutostartEngine() {
    pinMode(PIN_RELAY_IGN1, OUTPUT);
    pinMode(PIN_RELAY_IGN2, OUTPUT);
    pinMode(PIN_RELAY_ST, OUTPUT);

    digitalWrite(PIN_RELAY_IGN1, LOW);
    digitalWrite(PIN_RELAY_IGN2, LOW);
    digitalWrite(PIN_RELAY_ST, LOW);

    initSafetyInputs();
}

AutostartState getAutostartState() {
    return currentState;
}

void stopEngine(const char* reason) {
    digitalWrite(PIN_RELAY_ST, LOW);
    digitalWrite(PIN_RELAY_IGN2, LOW);
    digitalWrite(PIN_RELAY_IGN1, LOW);

    currentState = AUTOSTART_IDLE;
    Serial.printf("[AUTOSTART] Двигатель остановлен. Причина: %s\n", reason);
}

// 1. Предварительные проверки безопасности
static bool canStartEngine(bool isSystemArmed) {
    if (!isSystemArmed) {
        Serial.println("[AUTOSTART REJECT] Ошибка: Машина не на охране!");
        return false;
    }
    if (!isInParkPosition()) {
        Serial.println("[AUTOSTART REJECT] Ошибка: АКПП не в положении Park!");
        return false;
    }
    if (isBrakePressed()) {
        Serial.println("[AUTOSTART REJECT] Ошибка: Нажата педаль тормоза!");
        return false;
    }
    if (readBatteryVoltage() < 11.5f) {
        Serial.println("[AUTOSTART REJECT] Ошибка: Низкий заряд АКБ!");
        return false;
    }
    if (isEngineRunning()) {
        Serial.println("[AUTOSTART REJECT] Ошибка: Двигатель уже работает!");
        return false;
    }
    return true;
}

// 2. Логика запуска с повторными попытками
bool startAutostartSequence(bool isSystemArmed) {
    if (!canStartEngine(isSystemArmed)) {
        return false;
    }

    Serial.println("[AUTOSTART] Начало цикла автозапуска...");

    for (int attempt = 0; attempt < 3; attempt++) {
        Serial.printf("[AUTOSTART] Попытка %d из 3 (Макс. время: %d мс)\n", attempt + 1, starterAttemptDurations[attempt]);

        // Включаем зажигание
        digitalWrite(PIN_RELAY_IGN1, HIGH);
        digitalWrite(PIN_RELAY_IGN2, HIGH);
        delay(2000); // Ожидание накачки бензонасоса и инициализации ЭБУ

        // Отключаем печку/климат на время прокрутки стартера
        digitalWrite(PIN_RELAY_IGN2, LOW);
        digitalWrite(PIN_RELAY_ST, HIGH);

        unsigned long starterStart = millis();
        bool success = false;

        // Прокрутка стартера с непрерывной отсечкой по тахометру
        while (millis() - starterStart < starterAttemptDurations[attempt]) {
            if (getEngineRPM() >= ENGINE_RUN_RPM) {
                success = true;
                break;
            }
            delay(10);
        }

        // Мгновенно отключаем стартер
        digitalWrite(PIN_RELAY_ST, LOW);

        if (success) {
            delay(500);
            digitalWrite(PIN_RELAY_IGN2, HIGH); // Возвращаем питание климата
            engineStartTimestamp = millis();
            currentState = AUTOSTART_RUNNING;
            Serial.println("[AUTOSTART] Двигатель успешно запущен!");
            return true;
        }

        // Если не завелась — обесточиваем зажигание и пауза перед следующей попыткой
        digitalWrite(PIN_RELAY_IGN1, LOW);
        Serial.println("[AUTOSTART] Попытка не удалась.");

        if (attempt < 2) {
            delay(8000); // Пауза 8 секунд между попытками для восстановления АКБ
        }
    }

    stopEngine("Все 3 попытки запуска исчерпаны");
    return false;
}

// 3. Аварийный глушитель и Key Takeover (вызывается в main loop)
void processAutostartSafetyLoop() {
    if (currentState != AUTOSTART_RUNNING) {
        return;
    }

    // --- АВАРИЙНЫЕ БЛОКИРОВКИ ---
    
    // Потеря сигнала тахометра (мотор заглох)
    if (!isEngineRunning()) {
        stopEngine("Двигатель заглушен (потеря оборотов)");
        return;
    }

    // Выход напряжения за безопасные пределы
    float v = readBatteryVoltage();
    if (v < 10.8f || v > 15.5f) {
        stopEngine("Критическое напряжение бортовой сети");
        return;
    }

    // Селектор АКПП снят с Park до вставки ключа
    if (!isInParkPosition()) {
        stopEngine("Селектор АКПП переведен из положения P");
        return;
    }

    // Превышение времени прогрева (15 минут)
    if (millis() - engineStartTimestamp >= WARMUP_TIMEOUT_MS) {
        stopEngine("Истекло время прогрева (15 минут)");
        return;
    }

    // --- ЛОГИКА МЯГКОГО ПЕРЕХВАТА (KEY TAKEOVER) ---

    // Если нажата педаль тормоза...
    if (isBrakePressed()) {
        // ...и физический ключ в замке повернут в Положение II
        if (isKeyInIgnition()) {
            Serial.println("[KEY TAKEOVER] Ключ обнаружен в зажигании! Передача управления замку...");
            
            // Отключаем силовые реле ESP32, так как замок зажигания уже питает цепи напрямую
            digitalWrite(PIN_RELAY_IGN1, LOW);
            digitalWrite(PIN_RELAY_IGN2, LOW);
            
            currentState = AUTOSTART_KEY_TAKEOVER_COMPLETE;
            Serial.println("[KEY TAKEOVER] Перехват завершен. Машина работает от ключа.");
        } 
        else {
            // Если педаль тормоза нажата БЕЗ ключа в замке — это попытка угона!
            stopEngine("Нажатие тормоза без ключа в зажигании (Защита от угона)");
        }
    }
}