#include "Data/Measurement.h"

Measurement::Measurement() {}

Measurement::Measurement(bool ok, int value)
{
    this->ok = ok;
    this->value = value;
}

Measurement *Measurement::Failed()
{
    return new Measurement(false, 0);
}

Measurement *Measurement::Ok(int value)
{
    return new Measurement(true, value);
}