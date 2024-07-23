#pragma once

#include "Constants.h"

#include <cmath>

namespace Math
{

class Interval
{
public:
    double                Min;
    double                Max;
    static const Interval Empty;
    static const Interval Universe;

    // Default interval is empty
    Interval();
    Interval(const double min, const double max);
    Interval(const Interval& a, const Interval& b);

    /*
     * @brief Returns false if the interval is empty(inside out), or if either bound is a floating-point NaN (not a number).
     */
    bool IsEmpty() const;

    double   Size() const;
    Interval Expand(double delta) const;
    Interval Intersect(const Interval& other) const;
    bool     Contains(double x) const;
    bool     Surrounds(double x) const;
    double   Clamp(double x) const;

    /*
     * @brief Return the non-empty interval between a and b, regardless of their order.
     */
    static Interval Span(double a, double b);
};

inline Interval operator+(const Interval& intervalValue, const double displacement)
{
    return {intervalValue.Min + displacement, intervalValue.Max + displacement};
}

inline Interval operator+(const double displacement, const Interval& intervalValue)
{
    return intervalValue + displacement;
}

}    // namespace Math
