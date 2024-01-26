// ReSharper disable CppClangTidyModernizeUseNodiscard
#pragma once

#include <Math/Constants.h>

#include <cmath>

class Interval
{
public:
    double Min;
    double Max;

    Interval()
        : Min(+Infinity)
        , Max(-Infinity)
    {
    }    // Default interval is empty

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

    double Size() const
    {
        return Max - Min;
    }

    Interval Expand(const double delta) const
    {
        const auto padding = delta / 2;
        return {Min - padding, Max + padding};
    }

    bool Contains(const double x) const
    {
        return Min <= x && x <= Max;
    }

    bool Surrounds(const double x) const
    {
        return Min < x && x < Max;
    }

    double Clamp(const double x) const
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

    static const Interval Empty;
    static const Interval Universe;
};

const static Interval Empty(+Infinity, -Infinity);
const static Interval Universe(-Infinity, +Infinity);

inline Interval operator+(const Interval& intervalValue, const double displacement)
{
    return {intervalValue.Min + displacement, intervalValue.Max + displacement};
}

inline Interval operator+(const double displacement, const Interval& intervalValue)
{
    return intervalValue + displacement;
}
