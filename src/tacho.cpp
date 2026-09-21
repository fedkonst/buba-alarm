#include "tacho.h"

void initTacho() {
    // В будущем — настройка прерывания (attachInterrupt) на PIN_TACHO_IN
}

uint16_t getRPM() {
    return 0; // Для теста на столе возвращаем 0
}

bool isEngineRunning() {
    return false;
}
