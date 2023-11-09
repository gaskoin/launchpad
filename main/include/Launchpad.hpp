#ifndef LAUNCHPAD_H
#define LAUNCHPAD_H

#include <cstdint>

#include "ChargeEventListener.hpp"
#include "IODriver.hpp"
#include "EventDispatcher.hpp"
#include "IgnitionEventListener.hpp"
#include "Logger.hpp"

class Launchpad : public IgnitionEventListener, public ChargeEventListener
{
private:
    static Logger *log;

    IODriver *driver;
    LaunchStatus status = LaunchStatus::IDLE;

    void setIdle();
public:
    Launchpad(IODriver *driver, EventDispatcher *eventDispatcher);
    ~Launchpad(){};

    Launchpad(const Launchpad &) = delete;

    LaunchStatus getStatus();

    virtual void onCharge() override;
    virtual void onDischarge() override;
    virtual void onIgnition() override;
    void onArm();
    void onScheduleIgnition(int32_t timeToIgnition);
    void onAbort();
};

#endif