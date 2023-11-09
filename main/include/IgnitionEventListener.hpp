#ifndef IGNITION_EVENT_LISTENER_H
#define IGNITION_EVENT_LISTENER_H

class IgnitionEventListener
{
public:
    virtual ~IgnitionEventListener() {};

    virtual void onIgnition() = 0;
};

#endif