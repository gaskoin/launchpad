#ifndef CHARGE_EVENT_LISTENER_H
#define CHARGE_EVENT_LISTENER_H

class ChargeEventListener
{
public:
    virtual ~ChargeEventListener() {};

    virtual void onCharge() = 0;
    virtual void onDischarge() = 0;
};

#endif