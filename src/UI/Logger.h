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

    // This class allows printing to a char*, so we can leverage standard
    // Printable support for classes. It's up to the caller to provide a
    // buffer that is large enough to hold the results. When the printed
    // output is larger than the provided buffer, the overflow is silently
    // ignored.
    class StringPrinter : public Print
    {
    public:
        StringPrinter(char *buffer, int buflen) : _buf(buffer), _pos(0), _len(buflen)
        {
            if (_len > 0)
            {
                _buf[0] = '\0';
            }
        }

        virtual size_t write(uint8_t c)
        {
            // Silently ignore buffer overflows.
            if ((_pos + 1) < _len)
            {
                _buf[_pos] = c;
                _pos++;
                _buf[_pos] = '\0';
            }
            return 1;
        }

    private:
        char *_buf;
        size_t _pos;
        size_t _len;
    };
}

#endif