#include "shift.hpp"
#include <Arduino.h>

ShiftRegister::ShiftRegister(int data, int clock)
{
    dataPin = data;
    clockPin = clock;
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
}

void ShiftRegister::set(int value)
{
    shiftOut(dataPin, clockPin, MSBFIRST, value);
    this->value = value;
}
