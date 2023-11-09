#ifndef IO_DRIVER_H
#define IO_DRIVER_H

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include "IgnitionEventListener.hpp"

class IODriver
{
private:
    TimerHandle_t indicatorTimer;
    TimerHandle_t buzzerTimer;
    TimerHandle_t buzzerMuteTimer;
    TimerHandle_t ignitionDelayTimer;

    IODriver();

    IODriver(const IODriver &) = delete;

    void gpioInit();

public:
    static IODriver *init();

    void setStatusIndicator(bool level);
    void startBlinkingStatus(int periodInMs);
    void beepFor(int timeInMs);
    void startBeeping(int periodInMs);
    void igniteIn(int delayTimeInMs, IgnitionEventListener *ignitionEventListener);
    void ignite();
    void restartDevice(int delayTimeInMs = 0);
};

#endif