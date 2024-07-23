#include "Converters.h"

#include <algorithm>

namespace Math
{

double DoubleUtils::Epsilon()
{
    // return std::numeric_limits<double>::epsilon();
    return 1e-8;
}

double DoubleUtils::DefaultTolerance()
{
    // return 1e-6;
    return 1e-5;
}

double DoubleUtils::BigTolerance()
{
    return 1e-3;
}

bool DoubleUtils::isEqual(const double lhs, const double rhs, const double tolerance)
{
    const double difference = std::abs(lhs - rhs);
    if(difference <= tolerance)
    {
        return true;
    }

    return false;
}

bool DoubleUtils::isGreaterThanOrEqual(const double lhs, const double rhs, const double absoluteEpsilon)
{
    return isEqual(lhs, rhs, absoluteEpsilon) || lhs > rhs;
}

bool DoubleUtils::isLessThanOrEqual(const double lhs, const double rhs, const double absoluteEpsilon)
{
    return isEqual(lhs, rhs, absoluteEpsilon) || lhs < rhs;
}

bool DoubleUtils::isValueInRange(const double value, const double lowBound, const double upBound, const double absoluteEpsilon)
{
    return isLessThanOrEqual(lowBound, value, absoluteEpsilon) && isLessThanOrEqual(value, upBound, absoluteEpsilon);
}

}    // namespace Math
