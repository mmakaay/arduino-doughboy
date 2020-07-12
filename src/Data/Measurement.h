#ifndef DOUGH_DATA_MEASUREMENT_H
#define DOUGH_DATA_MEASUREMENT_H

namespace Dough
{
    // This class represents a single measurement, which can be either a
    // successful (bearing a measurement value) or a failed one.
    class Measurement
    {
    public:
        Measurement();
        int value = 0;
        bool ok = false;
        static Measurement Failed();
        static Measurement Value(int value);
        void clear();
        void copyTo(Measurement *target);
    };
}

#endif
