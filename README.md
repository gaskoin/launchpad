# Launchpad

This repository contains the device firmware for rocket launching.

 [TTGO T-Beam V1.2](https://www.aliexpress.com/item/1005003088139358.html) with 868MHz radio and custom buzzer and igniter modules are used as a hardware.

### TODO
- Low battery status
- Consider migration to [Lora](https://en.wikipedia.org/wiki/LoRa) or other non proprietary technology (BT, or simple REST api)
- folders and their description
- move platform dependent code to common folder

### Porting guide:
Code is written with portability in mind and porting mainly require removing usage of esp library and replacing it with other MCU one in listed files. 

Some concepts may need complete redesign as proprietary [esp-now](https://github.com/espressif/esp-now) is used in the project for ground-control communication and interfaces may not fit for other solutions.

#### Platform dependent code:
- [Power.hpp](https://github.com/gaskoin/ground-control/blob/main/main/include/Power.hpp) and [Power.cpp](https://github.com/gaskoin/ground-control/blob/main/main/Power.cpp) -> probably useless in other projects not using AXP2101 PMU.
- [Logger.cpp](https://github.com/gaskoin/ground-control/blob/main/main/Logger.cpp)
- [IODriver.cpp](https://github.com/gaskoin/ground-control/blob/main/main/IODriver.cpp)
- [DigitalInput.cpp](https://github.com/gaskoin/ground-control/blob/main/main/DigitalInput.cpp)
- [Communication.hpp](https://github.com/gaskoin/ground-control/blob/main/main/include/Communication.hpp) and [Communication.cpp](https://github.com/gaskoin/ground-control/blob/main/main/Communication.cpp)