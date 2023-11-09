#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <sstream>

#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_wifi.h"

#include "Communication.hpp"
#include "Config.hpp"

/*
 * Unfortunatelly there is no other way since ESP does not provide additional parameters
 * in esp_now_register_send_cb and esp_now_register_recv_cb
 */
static EventDispatcher *dispatcher;
static const int ADDRESS_SIZE = 6;
static const std::vector<uint8_t> BROADCAST_MAC {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static void onDataReceived(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int length);

Logger *Communication::log = Logger::getInstance();

void Communication::nvsInit()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void Communication::wifiInit()
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(Config::getWifiMode()));
    ESP_ERROR_CHECK(esp_wifi_start());
    log->info(TAG, "Setting channel %d", Config::getWifiChannel());
    ESP_ERROR_CHECK(esp_wifi_set_channel(Config::getWifiChannel(), WIFI_SECOND_CHAN_NONE));
    ESP_ERROR_CHECK(esp_wifi_set_protocol(Config::getWifiInterface(), WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR));
}

void Communication::espnowInit()
{
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(onDataReceived));
    ESP_ERROR_CHECK(esp_now_set_pmk(Config::getPrimaryMasterKey().data()));
}

Communication *Communication::init(EventDispatcher *eventDispatcher)
{
    Communication *communication = new Communication(eventDispatcher);
    dispatcher = eventDispatcher;

    communication->nvsInit();
    communication->wifiInit();
    communication->espnowInit();
    communication->registerPeer(BROADCAST_MAC);

    return communication;
}

Communication::Communication(EventDispatcher *eventDispatcher)
{
    eventDispatcher->appendListener<BroadcastEvent>(
        EventId::BROADCAST,
        [=, this](BroadcastEvent *event) { this->connect(event->address); } 
    );
}

void Communication::connect(uint8_t *address)
{
    log->info(TAG, "Received BROADCAST from launch control");
    if (isBroadcast(address))
    {
        log->error(TAG, "Cannot register peer as broadcast. Only unicast communication is allowed");
        return;
    }

    setLaunchControlAddress(address);
    pair(address);
}

void Communication::pair(uint8_t *address)
{
    log->info(TAG, "Pairing with launch control");
    
    std::vector<uint8_t> launchpadAddress = getAddress();

    PairEvent pairEvent;
    pairEvent.eventId = OutboundEventId::PAIR;
    std::copy(launchpadAddress.begin(), launchpadAddress.end(), pairEvent.address);

    sendToAddress<PairEvent>(BROADCAST_MAC, &pairEvent, sizeof(PairEvent));
}

void Communication::setLaunchControlAddress(uint8_t *address)
{
    std::vector<uint8_t> vectorAddress(address, address + ADDRESS_SIZE);
    if (!launchControlAddress.empty())
        return;

    launchControlAddress = vectorAddress;
    registerPeer(launchControlAddress);
}

bool Communication::isBroadcast(uint8_t *address)
{
    return isBroadcast(std::vector<uint8_t>(address, address + ADDRESS_SIZE));
}

bool Communication::isBroadcast(std::vector<uint8_t> address)
{
    return address == BROADCAST_MAC;
}

void Communication::registerPeer(std::vector<uint8_t> address)
{
    if (esp_now_is_peer_exist(address.data()))
        return;
        
    bool isBroadcast = this->isBroadcast(address);
    esp_now_peer_info_t peer;
    peer.channel = 1;
    peer.ifidx = Config::getWifiInterface();
    peer.encrypt = !isBroadcast;
    std::copy(address.begin(), address.end(), std::begin(peer.peer_addr));

    if (!isBroadcast)
    {
        std::vector<uint8_t> lmk = Config::getLocalMasterKey();
        std::copy(lmk.begin(), lmk.end(), std::begin(peer.lmk));
    }

    log->info(TAG, "Registering new peer %02x:%02x:%02x:%02x:%02x:%02x on channel %d", MAC2STR(peer.peer_addr), peer.channel);
    esp_now_add_peer(&peer);
}

std::vector<uint8_t> Communication::getAddress()
{
    if (address.empty())
    {
        uint8_t buffer[ADDRESS_SIZE];
        esp_read_mac(buffer, Config::getMacType());
        address.insert(address.end(), buffer, &(buffer[ADDRESS_SIZE]));
    }

    return address;
}

static void onDataReceived(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int length)
{
    if (dispatcher != nullptr)
        dispatcher->dispatch(data, length);
}