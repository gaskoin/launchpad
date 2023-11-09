#include "driver/gpio.h"

#include "DigitalInput.hpp"

DigitalInput::DigitalInput(int pinNumber) : pinNumber(pinNumber)
{
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << pinNumber);
    io_conf.mode = GPIO_MODE_INPUT;
    gpio_config(&io_conf);
}

bool DigitalInput::getLevel()
{
    return gpio_get_level((gpio_num_t)pinNumber);
}