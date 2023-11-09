#ifndef POWER_H
#define POWER_H

#include "freertos/FreeRTOS.h"

#define XPOWERS_CHIP_AXP2101
#include "XPowersLib.h"

#include "Logger.hpp"

class Power
{
private:
    static Logger *log;

    XPowersPMU *pmu;

    Power();
    ~Power() {};
    Power(const Power &) = delete;

    void pmuInit();

public:
    static Power *init();

    uint16_t getBatteryVoltage();
};

#endif