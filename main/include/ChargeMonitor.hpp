#ifndef CHARGE_MONITOR_H
#define CHARGE_MONITOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ChargeEventListener.hpp"
#include "DigitalInput.hpp"
#include "Logger.hpp"

class ChargeMonitor
{
private:
    static Logger *log;
    
    static void task(void *parameters);

    ChargeEventListener *listener;
    DigitalInput charge;

    void run();
public:
    ChargeMonitor(DigitalInput &charge);
    ChargeMonitor(const ChargeMonitor &) = delete;
    
    void start();
    void registerListener(ChargeEventListener *listener);
};
#endif