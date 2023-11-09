#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Config.hpp"
#include "ChargeMonitor.hpp"

static const char *TAG = "ChargeMonitor";
Logger *ChargeMonitor::log = Logger::getInstance();

ChargeMonitor::ChargeMonitor(DigitalInput &charge) : charge(std::move(charge)) {}

void ChargeMonitor::run()
{
    log->info(TAG, "Starting Charge Monitor");
    bool wasChargedBefore = false;

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(Config::getChargeMonitorPeriod()));

        if (listener == nullptr)
            continue;

        bool isCharged = charge.getLevel();
        if (isCharged && !wasChargedBefore)
            listener->onCharge();
        else if (!isCharged && wasChargedBefore)
            listener->onDischarge();

        wasChargedBefore = isCharged;
    }
}

void ChargeMonitor::registerListener(ChargeEventListener *listener)
{
    this->listener = listener;
}

void ChargeMonitor::start()
{
    xTaskCreate(ChargeMonitor::task, "charge_monitor", 4096, this, 4, nullptr);
}

void ChargeMonitor::task(void *parameters)
{
    static_cast<ChargeMonitor *>(parameters)->run();
}