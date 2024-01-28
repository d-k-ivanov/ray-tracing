#include "Interval.h"

// const static Interval Empty(+Infinity, -Infinity);
// const static Interval Universe(-Infinity, +Infinity);

const Interval Interval::Empty    = Interval(+Infinity, -Infinity);
const Interval Interval::Universe = Interval(-Infinity, +Infinity);

double Interval::Size() const
{
    return Max - Min;
}

Interval Interval::Expand(const double delta) const
{
    const auto padding = delta / 2;
    return {Min - padding, Max + padding};
}

bool Interval::Contains(const double x) const
{
    return Min <= x && x <= Max;
}

bool Interval::Surrounds(const double x) const
{
    return Min < x && x < Max;
}

double Interval::Clamp(const double x) const
{
    if(x < Min)
    {
        return Min;
    }
    if(x > Max)
    {
        return Max;
    }
    return x;
}
