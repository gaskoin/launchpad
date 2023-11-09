#include "driver/gpio.h"

#include "Config.hpp"
#include "IODriver.hpp"

#define INDICATOR_PIN GPIO_NUM_4
#define BUZZER_PIN GPIO_NUM_2
#define IGNITER_PIN GPIO_NUM_14

static void blinkStatusIndicator(TimerHandle_t timer);
static void switchBuzzer(TimerHandle_t timer);
static void muteBuzzer(TimerHandle_t timer);
static void callIgnition(TimerHandle_t timer);
static void onRestart(TimerHandle_t timer);

IODriver::IODriver()
{
    // TODO: consider IO class instead of big Facade
    this->indicatorTimer = xTimerCreate("indicator_timer", 1, true, nullptr, blinkStatusIndicator);
    this->buzzerTimer = xTimerCreate("buzzer_timer", 1, true, nullptr, switchBuzzer);
    this->buzzerMuteTimer = xTimerCreate("buzzer_mute_timer", 1, false, this, muteBuzzer);
    this->ignitionDelayTimer = xTimerCreate("ignition_delay_timer", 1, false, nullptr, callIgnition);
}

void IODriver::gpioInit()
{
    gpio_config_t io_conf = {};

    io_conf.pin_bit_mask = (1ULL << INDICATOR_PIN) | (1ULL << BUZZER_PIN) | (1ULL << IGNITER_PIN);
    io_conf.mode = GPIO_MODE_OUTPUT;
    gpio_config(&io_conf);

    gpio_set_level(IGNITER_PIN, 0);
    gpio_set_level(BUZZER_PIN, 0);
    gpio_set_level(INDICATOR_PIN, 0);
}

IODriver *IODriver::init()
{
    IODriver *driver = new IODriver();
    driver->gpioInit();
    return driver;
}

void IODriver::setStatusIndicator(bool level)
{
    xTimerStop(indicatorTimer, 100);
    gpio_set_level(INDICATOR_PIN, 1 - level);
}

void IODriver::startBlinkingStatus(int periodInMs)
{
    xTimerStop(indicatorTimer, 100);
    xTimerChangePeriod(indicatorTimer, pdMS_TO_TICKS(periodInMs), 100);
    xTimerStart(indicatorTimer, 100);
}

void IODriver::beepFor(int timeInMs)
{
    xTimerStop(buzzerTimer, 100);
    gpio_set_level(BUZZER_PIN, true);
    xTimerReset(buzzerMuteTimer, 100);
    xTimerChangePeriod(buzzerMuteTimer, pdMS_TO_TICKS(timeInMs), 100);
    xTimerStart(buzzerMuteTimer, 100);
}

void IODriver::startBeeping(int periodInMs)
{
    xTimerStop(buzzerTimer, 100);
    xTimerChangePeriod(buzzerTimer, pdMS_TO_TICKS(periodInMs), 100);
    xTimerStart(buzzerTimer, 100);
}

void IODriver::igniteIn(int delayTimeInMs, IgnitionEventListener *ignitionEventListener)
{
    vTimerSetTimerID(ignitionDelayTimer, ignitionEventListener);
    xTimerChangePeriod(ignitionDelayTimer, pdMS_TO_TICKS(delayTimeInMs), 100);
    xTimerStart(ignitionDelayTimer, 100);
}

void IODriver::ignite()
{
    gpio_set_level(IGNITER_PIN, 1);
}

void IODriver::restartDevice(int delayTimeInMs)
{
    if (delayTimeInMs <= 0)
        onRestart(nullptr);
        
    TimerHandle_t restartTimer = xTimerCreate("restart_timer", pdMS_TO_TICKS(delayTimeInMs), false, nullptr, onRestart);
    xTimerStart(restartTimer, 100);
}

static void muteBuzzer(TimerHandle_t timer)
{
    gpio_set_level(BUZZER_PIN, false);
}

static void callIgnition(TimerHandle_t timer)
{
    IgnitionEventListener *listener = static_cast<IgnitionEventListener *>(pvTimerGetTimerID(timer));
    listener->onIgnition();
}

static void blinkStatusIndicator(TimerHandle_t timer)
{
    static bool level = 0;

    gpio_set_level(INDICATOR_PIN, level);
    level = 1 - level;
}

static void switchBuzzer(TimerHandle_t timer)
{
    static bool level = 0;

    gpio_set_level(BUZZER_PIN, level);
    level = 1 - level;
}

static void onRestart(TimerHandle_t timer)
{
    esp_restart();
}