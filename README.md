
<img src="https://github.com/gaskoin/launchpad/assets/5610300/f2ad325b-41ff-4f83-921a-f6ba6270ad64.png" height=300 align="left" />

# Launchpad
This repository contains the device firmware for rocket launching.

[TTGO T-Beam V1.2](https://www.aliexpress.com/item/1005003088139358.html) with 868MHz radio and custom buzzer and igniter modules are used as a hardware.

Support is build from 25 mm PP pipes with 45 degree knees and cross link. Defflector is made from 2 mm bent copper bar. 
Mechanical construction assumes small rockets for 3 mm launching rod but more power rockets can also be launched with this electronics. Bigger battery needs to be used and other mechanical construction. 
<br clear="left"/>

### TODO
- Low battery status
- Consider migration to [Lora](https://en.wikipedia.org/wiki/LoRa) or other non proprietary technology (BT, or simple REST api)
- folders and their description
- move platform dependent code to common folder

### Porting guide:
Code is written with portability in mind and porting mainly require removing usage of esp library and replacing it with other MCU one in listed files. 

Some concepts may need complete redesign as proprietary [esp-now](https://github.com/espressif/esp-now) is used in the project for ground-control communication and interfaces may not fit for other solutions.

#### Platform dependent code:
- [Power.hpp](https://github.com/gaskoin/launchpad/blob/main/main/include/Power.hpp) and [Power.cpp](https://github.com/gaskoin/launchpad/blob/main/main/Power.cpp) -> probably useless in other projects not using AXP2101 PMU.
- [Logger.cpp](https://github.com/gaskoin/launchpad/blob/main/main/Logger.cpp)
- [IODriver.cpp](https://github.com/gaskoin/launchpad/blob/main/main/IODriver.cpp)
- [DigitalInput.cpp](https://github.com/gaskoin/launchpad/blob/main/main/DigitalInput.cpp)
- [Communication.hpp](https://github.com/gaskoin/launchpad/blob/main/main/include/Communication.hpp) and [Communication.cpp](https://github.com/gaskoin/launchpad/blob/main/main/Communication.cpp)