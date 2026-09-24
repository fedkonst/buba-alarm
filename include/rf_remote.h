#ifndef RF_REMOTE_H
#define RF_REMOTE_H

#include <Arduino.h>
#include "config.h"
#include "states.h"

#define COURTESY_LIGHT_MS 15000  // Длительность вежливого света (15 сек)

enum RFCommand {
    RF_CMD_NONE,
    RF_CMD_LOCK,
    RF_CMD_UNLOCK,
    RF_CMD_TRUNK,
    RF_CMD_SEARCH
};

void rf_remote_init();
void rf_remote_loop(SystemState &state);
void courtesy_light_start();
void courtesy_light_loop();

#endif // RF_REMOTE_H