#include "Interval.h"

#include <algorithm>

namespace Math
{

// const static Interval Empty(+Infinity, -Infinity);
// const static Interval Universe(-Infinity, +Infinity);

const Interval Interval::Empty    = Interval(+Infinity, -Infinity);
const Interval Interval::Universe = Interval(-Infinity, +Infinity);

Interval::Interval()
    : Min(+Infinity)
    , Max(-Infinity)
{
}

Interval::Interval(const double min, const double max)
    : Min(min)
    , Max(max)
{
}

Interval::Interval(const Interval& a, const Interval& b)
    : Min(std::min(a.Min, b.Min))
    , Max(std::max(a.Max, b.Max))
{
    // New approach: https://github.com/RayTracing/raytracing.github.io/pull/1422
    // Min = a.Min <= b.Min ? a.Min : b.Min;
    // Max = a.Max >= b.Max ? a.Max : b.Max;
}

bool Interval::IsEmpty() const
{
    return !(Min <= Max);
}

double Interval::Size() const
{
    return Max - Min;
}

Interval Interval::Expand(const double delta) const
{
    const auto padding = delta / 2;
    return {Min - padding, Max + padding};
}

Interval Interval::Intersect(const Interval& other) const
{
    double a = Min >= other.Min ? Min : other.Min;
    double b = Max <= other.Max ? Max : other.Max;
    return {a, b};
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

Interval Interval::Span(double a, double b)
{
    if(a > b)
        return {b, a};
    return {a, b};
}

}    // namespace Math
