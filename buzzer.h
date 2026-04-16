#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
public:
    void begin(uint8_t pin = 33);

    void update();

    void beep(uint8_t count);
    void beep(uint8_t count, uint16_t onTime, uint16_t offTime);

    void beepLong(uint16_t duration);

    bool isRunning();

private:
    uint8_t _pin;

    uint8_t _count = 0;
    uint8_t _current = 0;

    uint16_t _onTime = 100;
    uint16_t _offTime = 100;

    uint32_t _timer = 0;

    bool _state = false;
    bool _running = false;

};

extern Buzzer buzzer;

#endif