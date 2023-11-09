#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include <functional>
#include <map>
#include <string>

#include "Events.hpp"
#include "Logger.hpp"

class EventDispatcher
{
private:
    static Logger *log;

    std::map<EventId, std::function<void(const uint8_t *)>> handlers;

public:
    EventDispatcher();
    ~EventDispatcher(){};
    
    EventDispatcher(const EventDispatcher &) = delete;

    template <typename T>
    void appendListener(EventId eventId, std::function<void(T *)> handler)
    {
        std::function<void(const uint8_t *)> handlerWrapper = [handler](const uint8_t *rawData)
        {
            T *parameter = const_cast<T *>(reinterpret_cast<const T *>(rawData));
            handler(parameter);
        };
        handlers[eventId] = handlerWrapper;
    }

    void dispatch(const uint8_t *data, int length);
};

#endif
