#include "DoughLogger.h"

DoughLogger::DoughLogger(const char *section)
{
    _section = section;
}

void DoughLogger::log(const char* fmt, ...)
{
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
        else if (*fmt == 'a')
        {
            IPAddress a = va_arg(args, IPAddress);
            Serial.print(a);
        }
        else if (*fmt == 's')
        {
            const char *s = va_arg(args, const char *);
            Serial.print(s);
        }
        else if (*fmt == 'S')
        {
            String S = va_arg(args, String);
            Serial.print(S);
        }
        else
        {
            Serial.print("<log(): invalid format char '");
            Serial.print(*fmt);
            Serial.print("'>");
        }
        fmt++;
    }
    va_end(args);

    Serial.println("");    
}
