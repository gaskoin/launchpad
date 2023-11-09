#include <cstdarg>
#include <cstdio>

#include "esp_log.h"
#include "logger.hpp"

static void log(esp_log_level_t logLevel, const char *tag, const char *format, va_list list);

Logger *Logger::instance = nullptr;

Logger::Logger() {}

Logger *Logger::getInstance()
{
    if (instance == nullptr)
        instance = new Logger();

    return instance;
}

void Logger::info(const char *tag, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log(ESP_LOG_INFO, tag, format, args);
    va_end(args);
}

void Logger::warning(const char *tag, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log(ESP_LOG_WARN, tag, format, args);
    va_end(args);
}

void Logger::error(const char *tag, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log(ESP_LOG_ERROR, tag, format, args);
    va_end(args);
}

static void log(esp_log_level_t logLevel, const char *tag, const char *format, va_list args)
{
    char temp[80];
    vsnprintf(temp, sizeof(temp), format, args);
    ESP_LOG_LEVEL_LOCAL(logLevel, tag, "%s", temp);
}