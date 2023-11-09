#ifndef LOGGER_H
#define LOGGER_H

class Logger
{
private:
    static Logger *instance;

    Logger();
    ~Logger(){};

    Logger(const Logger &) = delete;

public:
    static Logger *getInstance();
    void info(const char *tag, const char *format, ...);
    void warning(const char *tag, const char *format, ...);
    void error(const char *tag, const char *format, ...);
};

#endif
