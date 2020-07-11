#ifndef DOUGH_LOGGER_H
#define DOUGH_LOGGER_H

#define LOGGER_PREFIX_BUFLEN 16
#define LOGGER_PREFIX_FORMAT "%11s | "

#include <Arduino.h>
#include <stdarg.h>
#include <WiFiNINA.h>

/**
 * This class provides an interface for logging data in a structured
 * manner to the serial console.
 */
class DoughLogger
{
public:
    DoughLogger(const char *section);
    void log(const char *fmt, ...);
    void suspend();
    void resume();

private:
    const char *_section;
    bool _suspended = false;
};

#endif
