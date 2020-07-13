#ifndef DOUGH_LOGGER_H
#define DOUGH_LOGGER_H

#define LOGGER_PREFIX_BUFLEN 16
#define LOGGER_PREFIX_FORMAT "%11s | "

#include <Arduino.h>
#include <stdarg.h>
#include <WiFiNINA.h>
#include "config.h"

namespace Dough
{
    // This class provides an interface for logging data in a structured
    // manner to the serial console.
    class Logger
    {
    public:
        static void setup();
        Logger(const char *section);
        void log(const char *fmt, ...);
        void suspend();
        void resume();

    private:
        const char *_section;
        bool _suspended = false;
    };
}

#endif