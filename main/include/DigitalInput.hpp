#ifndef DIGITAL_INPUT_H
#define DIGITAL_INPUT_H

#include <functional>

class DigitalInput
{
private:
    int pinNumber;

public:
    DigitalInput(int pinNumber);
    ~DigitalInput(){};

    bool getLevel();
};

#endif