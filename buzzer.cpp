#include "buzzer.h"

void Buzzer::begin(uint8_t pin) {
    _pin = pin;
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, HIGH); // Kalau pakai Lewat Output Relay (Active LOW) Positif Buzzer Connect ke NC Relay 
    // digitalWrite(_pin, LOW); // Kalau pakai GPIO langsung
}

void Buzzer::update() {

    if (!_running) return;

    if (millis() - _timer >= (_state ? _onTime : _offTime)) {

        _timer = millis();

        _state = !_state;
        digitalWrite(_pin, _state);

        if (!_state) {
            _current++;

            if (_current >= _count) {
                _running = false;
                digitalWrite(_pin, LOW);
            }
        }
    }
}

void Buzzer::beep(uint8_t count) {
    beep(count, 100, 100);
}

void Buzzer::beep(uint8_t count, uint16_t onTime, uint16_t offTime) {

    if (count == 0) return;
    if (count > 10) count = 10;

    _count = count;
    _current = 0;

    _onTime = onTime;
    _offTime = offTime;

    _running = true;
    _state = false;
    _timer = millis();
}

void Buzzer::beepLong(uint16_t duration) {

    _count = 1;
    _current = 0;

    _onTime = duration;
    _offTime = 10;

    _running = true;
    _state = false;
    _timer = millis();
}

bool Buzzer::isRunning() {
    return _running;
}

Buzzer buzzer;