#pragma once

#include <Math/Constants.h>

#include <algorithm>

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
    static double Epsilon()
    {
        return std::numeric_limits<double>::epsilon();
    }

    static double DefaultTolerance()
    {
        return 1e-6;
    }

    static double BigTolerance()
    {
        return 1e-3;
    }

    static bool isEqual(const double lhs, const double rhs, const double tolerance = Epsilon())
    {
        const double difference = std::abs(lhs - rhs);
        if(difference <= tolerance)
        {
            return true;
        }

        return false;
    }

    static bool isGreaterThanOrEqual(const double lhs, const double rhs, const double absoluteEpsilon = Epsilon())
    {
        return isEqual(lhs, rhs, absoluteEpsilon) || lhs > rhs;
    }

    static bool isLessThanOrEqual(const double lhs, const double rhs, const double absoluteEpsilon = Epsilon())
    {
        return isEqual(lhs, rhs, absoluteEpsilon) || lhs < rhs;
    }

    static bool isValueInRange(const double value, const double lowBound, const double upBound, const double absoluteEpsilon = Epsilon())
    {
        return isLessThanOrEqual(lowBound, value, absoluteEpsilon) && isLessThanOrEqual(value, upBound, absoluteEpsilon);
    }
};