#include <string>

#include "EventDispatcher.hpp"

static const char *TAG = "EventDispatcher";
Logger *EventDispatcher::log = Logger::getInstance();

EventDispatcher::EventDispatcher() {}

void EventDispatcher::dispatch(const uint8_t *data, int length)
{
    EventId eventId = static_cast<EventId>(data[0]);
    if (!handlers.empty())
        handlers.at(eventId)(data);
    else
        log->warning(TAG, "No handler registered for eventId '%d'", eventId);
}