#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "Config.hpp"
#include "Events.hpp"
#include "HealthCheck.hpp"

static const char *TAG = "HealthCheck";
Logger *HealthCheck::log = Logger::getInstance();

HealthCheck::HealthCheck(Launchpad *launchpad, Power *power, Communication *communication)
{
    this->launchpad = launchpad;
    this->power = power;
    this->communication = communication;
}

void HealthCheck::run()
{
    log->info(TAG, "Starting health check");

    while (true)
    {
        HealthCheckEvent healthCheck = {
            .eventId = OutboundEventId::HEALTH_CHECK,
            .batteryVoltage = power->getBatteryVoltage(),
            .status = launchpad->getStatus()
        };

        log->info(TAG, "Battery : %.3f", healthCheck.batteryVoltage / (float)1000);
        log->info(TAG, "Status  : %d", healthCheck.status);

        communication->sendData<HealthCheckEvent>(&healthCheck, sizeof(HealthCheckEvent));
        vTaskDelay(pdMS_TO_TICKS(Config::getHealthCheckPeriod()));
    }
}

void HealthCheck::start()
{
    xTaskCreate(HealthCheck::task, "health_check", 4096, this, 4, NULL);
}

void HealthCheck::task(void *parameters)
{
    static_cast<HealthCheck *>(parameters)->run();
}