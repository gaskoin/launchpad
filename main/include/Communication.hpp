#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <cstdint>
#include <vector>
#include <string>

#include "esp_mac.h"
#include "esp_now.h"

#include "EventDispatcher.hpp"
#include "Logger.hpp"

class Communication
{
private:
    constexpr static char const *TAG = "Communication";
    static Logger *log;

    std::vector<uint8_t> address;
    std::vector<uint8_t> launchControlAddress;

    Communication(EventDispatcher *eventDispatcher);
    ~Communication(){};

    Communication(const Communication &) = delete;

    void nvsInit();
    void wifiInit();
    void espnowInit();

    void pair(uint8_t *address);
    void connect(uint8_t *address);
    std::vector<uint8_t> getAddress();
    void setLaunchControlAddress(uint8_t *address);
    bool isBroadcast(uint8_t *address);
    bool isBroadcast(std::vector<uint8_t> address);
    void registerPeer(std::vector<uint8_t> address);
    
    template <typename T>
    void sendToAddress(std::vector<uint8_t> address, T *data, int size)
    {
        uint8_t *rawData = reinterpret_cast<uint8_t *>(data);
        log->info(Communication::TAG, "Sending eventId = '%d' to %02x:%02x:%02x:%02x:%02x:%02x", rawData[0], MAC2STR(address.data()));
        esp_now_send(address.data(), rawData, size);
    }

public:
    static Communication *init(EventDispatcher *EventDispatcher);

    template <typename T>
    void sendData(T *data, int size)
    {
        if (launchControlAddress.empty())
        {
            log->warning(Communication::TAG, "Launch control is not connected");
            return;
        }

        sendToAddress(launchControlAddress, data, size);
    }
};

#endif