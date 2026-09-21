#pragma once
#ifndef STATES_H
#define STATES_H

enum SystemState {
    STATE_ARMED,
    STATE_DISARMED,
    STATE_AUTHENTICATED,
    STATE_ACC_ON,
    STATE_IGN_ON,
    STATE_STARTING,
    STATE_RUNNING,
    STATE_ALARM
};
void updateRelays(SystemState state);

#endif // STATES_H