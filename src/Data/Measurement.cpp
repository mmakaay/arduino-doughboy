#include "Data/Measurement.h"

Measurement::Measurement() { }

Measurement Measurement::Failed()
{
    Measurement m;
    return m;
}

Measurement Measurement::Value(int value)
{
    Measurement m;
    m.ok = true;
    m.value = value;
    return m;
}

void Measurement::clear()
{
    ok = false;
    value = 0;
}

void Measurement::copyTo(Measurement* target)
{
    target->ok = ok;
    target->value = value;
}