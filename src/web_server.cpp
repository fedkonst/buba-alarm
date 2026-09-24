#include "web_server.h"
#include "web_index.h"
#include "measurements.h"
#include "tachometer.h"
#include "autostart_engine.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Preferences.h>

static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");
static Preferences prefs;

// Внешние функции из main.cpp
extern void triggerEngineStart();
extern void triggerEngineStop(const char* reason);
extern void setArmedState(bool arm);
extern bool getSystemArmed();

static struct {
    bool autoLockDrive;
    String lockCondition;
    bool autoUnlockStop;
    String unlockCondition;
    int autostartTemp;
    float autostartVolt;
    int politeLightSec;
} sysConfig;

bool getSpeedLockEnabled() { return sysConfig.autoLockDrive; }
bool getUnlockOnStopEnabled() { return sysConfig.autoUnlockStop; }

static void loadSettings() {
    prefs.begin("telematics", true);
    sysConfig.autoLockDrive = prefs.getBool("al_drive", true);
    sysConfig.lockCondition = prefs.getString("al_cond", "speed");
    sysConfig.autoUnlockStop = prefs.getBool("au_stop", true);
    sysConfig.unlockCondition = prefs.getString("au_cond", "ign_off");
    sysConfig.autostartTemp = prefs.getInt("as_temp", -18);
    sysConfig.autostartVolt = prefs.getFloat("as_volt", 11.8f);
    sysConfig.politeLightSec = prefs.getInt("pl_sec", 30);
    prefs.end();
}

static void saveSettings(JsonObject doc) {
    prefs.begin("telematics", false);
    if (doc.containsKey("autoLockDrive")) prefs.putBool("al_drive", doc["autoLockDrive"]);
    if (doc.containsKey("lockCondition")) prefs.putString("al_cond", doc["lockCondition"].as<String>());
    if (doc.containsKey("autoUnlockStop")) prefs.putBool("au_stop", doc["autoUnlockStop"]);
    if (doc.containsKey("unlockCondition")) prefs.putString("au_cond", doc["unlockCondition"].as<String>());
    if (doc.containsKey("autostartTemp")) prefs.putInt("as_temp", doc["autostartTemp"]);
    if (doc.containsKey("autostartVolt")) prefs.putFloat("as_volt", doc["autostartVolt"]);
    if (doc.containsKey("politeLightSec")) prefs.putInt("pl_sec", doc["politeLightSec"]);
    prefs.end();
    loadSettings();
}

static void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, data, len);
    if (error) return;

    const char* cmd = doc["cmd"];
    if (strcmp(cmd, "engine") == 0) {
        bool reqState = doc["state"];
        if (reqState) {
            triggerEngineStart();
        } else {
            triggerEngineStop("Остановка с веб-панели");
        }
        updateWebTelemetry();
    }
    else if (strcmp(cmd, "security") == 0) {
        setArmedState(doc["state"]);
        updateWebTelemetry();
    }
    else if (strcmp(cmd, "save_config") == 0) {
        saveSettings(doc.as<JsonObject>());
        StaticJsonDocument<128> resp;
        resp["event"] = "config_saved";
        String out;
        serializeJson(resp, out);
        ws.textAll(out);
    }
}

static void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                      void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_DATA) {
        handleWebSocketMessage(arg, data, len);
    } else if (type == WS_EVT_CONNECT) {
        updateWebTelemetry();
    }
}

void initWebServer() {
    loadSettings();

    // Создаем Wi-Fi точку (или используй WiFi.begin для роутера)
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Volvo_850_DX", "volvo1993");

    // Отдача веб-интерфейса из Flash (PROGMEM)
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", PAGE_INDEX);
    });

    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    server.begin();
    Serial.println("[HTTP] Web-сервер и WebSockets запущены на http://192.168.4.1");
}

void updateWebTelemetry() {
    if (ws.count() == 0) return;

    StaticJsonDocument<256> doc;
    AutostartState st = getAutostartState();
    bool running = (st == AUTOSTART_RUNNING || st == AUTOSTART_KEY_TAKEOVER_COMPLETE);

    doc["event"] = "telemetry";
    doc["engineRunning"] = running;
    doc["isArmed"] = getSystemArmed();
    doc["rpm"] = getEngineRPM();
    doc["vBat"] = readBatteryVoltage();
    doc["tCoolant"] = 87; // Если есть датчик в measurements.h, подставь его
    doc["tCabin"] = 21;   // Температура салона

    String json;
    serializeJson(doc, json);
    ws.textAll(json);
}