#pragma once

#include "Constants.h"

inline double DegreesToRadians(const double degrees)
{
    return degrees * Pi / 180.0;
}

inline double RadiansToDegrees(const double degrees)
{
    return degrees * 180.0 / Pi;
}

class DoubleUtils
{
public:
    static double Epsilon();
    static double DefaultTolerance();
    static double BigTolerance();
    static bool   isEqual(double lhs, double rhs, double tolerance = Epsilon());
    static bool   isGreaterThanOrEqual(double lhs, double rhs, double absoluteEpsilon = Epsilon());
    static bool   isLessThanOrEqual(double lhs, double rhs, double absoluteEpsilon = Epsilon());
    static bool   isValueInRange(double value, double lowBound, double upBound, double absoluteEpsilon = Epsilon());
};