#ifndef DOUGH_DATA_MEASUREMENT_H
#define DOUGH_DATA_MEASUREMENT_H

/**
 * The DoughDataMeasurement class represents a single measurement.
 */
class Measurement
{
public:
    Measurement();
    Measurement(bool ok, int value);
    int value = 0;
    bool ok = false;
    static Measurement *Failed();
    static Measurement *Ok(int value);
};

#endif
