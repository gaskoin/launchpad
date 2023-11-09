#ifndef EVENTS_H
#define EVENTS_H

#include <cstdint>

enum class EventId
{
    BROADCAST,
    ARM,
    START_IGNITION,
    ABORT
};

typedef struct
{
    EventId eventId;
    uint8_t address[6];
} BroadcastEvent;

typedef struct
{
    EventId eventId;
} ArmEvent;

typedef struct
{
    EventId eventId;
    uint16_t delay;
} IgniteEvent;

typedef struct
{
    EventId eventId;
} AbortEvent;

enum class OutboundEventId
{
    ACK,
    BROADCAST,
    PAIR,
    HEALTH_CHECK
};

enum class LaunchStatus
{
    IDLE,
    CHARGED,
    ARMED,
    IGNITION
};

typedef struct
{
    OutboundEventId eventId;
    uint8_t address[6];
} OutboundBroadcastEvent;

typedef struct
{
    OutboundEventId eventId;
    uint8_t address[6];
} PairEvent;

typedef struct
{
    OutboundEventId eventId;
    uint16_t batteryVoltage;
    LaunchStatus status;
} HealthCheckEvent;

#endif