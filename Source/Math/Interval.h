#pragma once

#include <Math/Constants.h>

#include <cmath>

class Interval
{
public:
    double                Min;
    double                Max;
    static const Interval Empty;
    static const Interval Universe;

    // Default interval is empty
    Interval()
        : Min(+Infinity)
        , Max(-Infinity)
    {
    }

    Interval(const double min, const double max)
        : Min(min)
        , Max(max)
    {
    }

    Interval(const Interval& a, const Interval& b)
        : Min(std::fmin(a.Min, b.Min))
        , Max(std::fmax(a.Max, b.Max))
    {
    }

    double   Size() const;
    Interval Expand(double delta) const;
    bool     Contains(double x) const;
    bool     Surrounds(double x) const;
    double   Clamp(double x) const;
};

inline Interval operator+(const Interval& intervalValue, const double displacement)
{
    return {intervalValue.Min + displacement, intervalValue.Max + displacement};
}

inline Interval operator+(const double displacement, const Interval& intervalValue)
{
    return intervalValue + displacement;
}
