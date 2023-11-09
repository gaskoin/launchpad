#include <iostream>

#include "Config.hpp"
#include "Launchpad.hpp"

static const char *TAG = "Launchpad";
Logger *Launchpad::log = Logger::getInstance();

Launchpad::Launchpad(IODriver *driver, EventDispatcher *eventDispatcher) : driver(driver)
{
    eventDispatcher->appendListener<ArmEvent>(
        EventId::ARM,
        [=, this](ArmEvent *event) { this->onArm(); }
    );

    eventDispatcher->appendListener<IgniteEvent>(
        EventId::START_IGNITION,
        [=, this](IgniteEvent *event) { this->onScheduleIgnition(event->delay); }
    );

    eventDispatcher->appendListener<AbortEvent>(
        EventId::ABORT,
        [=, this](AbortEvent *event) { this->onAbort(); } 
    );

    setIdle();
}

void Launchpad::setIdle()
{
    status = LaunchStatus::IDLE;
    driver->setStatusIndicator(false);
    driver->beepFor(Config::getIdleBeepTime());
}

void Launchpad::onCharge()
{
    status = LaunchStatus::CHARGED;

    driver->setStatusIndicator(true);
    driver->beepFor(Config::getChargeBeepTime());
    log->info(TAG, "Charge connected to launchpad");
}

// this may occur due to all current path through MOSFET
void Launchpad::onDischarge()
{
    if (status == LaunchStatus::IGNITION)
        return;

    setIdle();
}

void Launchpad::onArm()
{
    if (status == LaunchStatus::IDLE)
    {
        log->warning(TAG, "Cannot arm - charge is not connected to the launchpad");
        return;
    }

    if (status != LaunchStatus::CHARGED)
        return;

    status = LaunchStatus::ARMED;
    driver->startBlinkingStatus(Config::getArmedIndicatorsPeriod());
    driver->startBeeping(Config::getArmedIndicatorsPeriod());
    log->info(TAG, "Arming the charge");
}

void Launchpad::onScheduleIgnition(int32_t timeToIgnition)
{
    if (status != LaunchStatus::ARMED)
    {
        log->warning(TAG, "Launchpad should be armed first");
        return;
    }

    if (timeToIgnition <= 0)
    {
        log->warning(TAG, "Time to ignition (%d ms) is too short. Suggested time is 5000-10000 ms", timeToIgnition);
        return;
    }

    status = LaunchStatus::IGNITION;

    driver->startBlinkingStatus(Config::getPendingIgniotionIndicatorsPeriod());
    driver->startBeeping(Config::getPendingIgniotionIndicatorsPeriod());
    driver->igniteIn(timeToIgnition, this);
    log->info(TAG, "Ignition in %d ms", timeToIgnition);
}

void Launchpad::onIgnition()
{
    driver->setStatusIndicator(true);
    driver->beepFor(Config::getIgnitionBeepTime());
    driver->ignite();
    log->info(TAG, "Ignition!");

    driver->restartDevice(Config::getRestartDelay());
}

void Launchpad::onAbort()
{
    log->info(TAG, "Aborting all operations... restarting launchpad");
    driver->restartDevice();
}

LaunchStatus Launchpad::getStatus()
{
    return status;
}