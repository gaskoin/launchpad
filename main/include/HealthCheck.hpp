#ifndef HEALTH_CHECK_H
#define HEALTH_CHECK_H

#include "Communication.hpp"
#include "Launchpad.hpp"
#include "Power.hpp"

class HealthCheck
{
private:
    static Logger *log;
    static void task(void *parameters);

    Launchpad *launchpad;
    Power *power;
    Communication *communication;

    void run();
public:
    HealthCheck(Launchpad *launchpad, Power *power, Communication *communication);
    ~HealthCheck(){};

    HealthCheck(const HealthCheck &) = delete;

    void start();
};

#endif