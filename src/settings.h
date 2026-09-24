#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <Preferences.h>

// Структура хранящихся настроек
struct SystemSettings {
    uint16_t crankTimeMs = 1500;
    uint8_t maxWarmupMinutes = 15;
    float minVoltage = 11.8f;
    uint8_t lightTimeSec = 30;
};

inline SystemSettings currentSettings;

// Инициализация и чтение из NVS (Preferences)
inline void initSettings() {
    Preferences prefs;
    prefs.begin("telematics", false);
    currentSettings.crankTimeMs = prefs.getUInt("crank", 1500);
    currentSettings.maxWarmupMinutes = prefs.getUChar("warmup", 15);
    currentSettings.minVoltage = prefs.getFloat("minv", 11.8f);
    currentSettings.lightTimeSec = prefs.getUChar("light", 30);
    prefs.end();
}

// Получить текущие настройки
inline SystemSettings getSettings() {
    return currentSettings;
}

// Сохранить новые настройки в NVS
inline void saveSettings(const SystemSettings& st) {
    currentSettings = st;
    Preferences prefs;
    prefs.begin("telematics", false);
    prefs.putUInt("crank", st.crankTimeMs);
    prefs.putUChar("warmup", st.maxWarmupMinutes);
    prefs.putFloat("minv", st.minVoltage);
    prefs.putUChar("light", st.lightTimeSec);
    prefs.end();
}

#endif