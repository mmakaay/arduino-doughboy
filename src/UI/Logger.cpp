#include "Logger.h"

namespace Dough
{
    Logger::Logger(const char *section)
    {
        _section = section;
    }

    void Logger::setup()
    {
        // Setup the serial port, used for logging.
        Serial.begin(LOG_BAUDRATE);
#ifdef LOG_WAIT_SERIAL
        while (!Serial)
        {
            // wait for serial port to connect. Needed for native USB.
        }
#endif
    }

    void Logger::suspend()
    {
        _suspended = true;
    }

    void Logger::resume()
    {
        _suspended = false;
    }

    void Logger::log(const char *fmt, ...)
    {
        if (_suspended)
        {
            return;
        }

        char buf[LOGGER_PREFIX_BUFLEN];
        snprintf(buf, sizeof(buf) / sizeof(buf[0]), LOGGER_PREFIX_FORMAT, _section);
        Serial.print(buf);

        va_list args;
        va_start(args, fmt);

        while (*fmt != '\0')
        {
            if (*fmt == 'i')
            {
                int i = va_arg(args, int);
                Serial.print(i);
            }
            else if (*fmt == 'f')
            {
                float f = va_arg(args, double);
                Serial.print(f);
            }
            else if (*fmt == 's')
            {
                const char *s = va_arg(args, const char *);
                Serial.print(s);
            }
            else
            {
                Serial.print("<log(): invalid format char '");
                Serial.print(*fmt);
                Serial.println("'>");
                return;
            }
            fmt++;
        }
        va_end(args);

        Serial.println("");
    }
} // namespace Dough