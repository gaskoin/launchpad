#include "driver/gpio.h"

#include "ChargeMonitor.hpp"
#include "Communication.hpp"
#include "DigitalInput.hpp"
#include "EventDispatcher.hpp"
#include "IODriver.hpp"
#include "HealthCheck.hpp"
#include "Launchpad.hpp"
#include "Power.hpp"

#define CHARGE_SENSE_PIN GPIO_NUM_13

extern "C" void app_main(void)
{
    IODriver *driver = IODriver::init();
    Power *power = Power::init();
    EventDispatcher *eventDispatcher = new EventDispatcher();
    Communication *communication = Communication::init(eventDispatcher);

    Launchpad *launchpad = new Launchpad(driver, eventDispatcher);
    
    DigitalInput chargeSense = DigitalInput(CHARGE_SENSE_PIN);
    ChargeMonitor *chargeMonitor = new ChargeMonitor(chargeSense);
    HealthCheck *healthCheck = new HealthCheck(launchpad, power, communication);

    chargeMonitor->registerListener(launchpad);

    healthCheck->start();
    chargeMonitor->start();
}