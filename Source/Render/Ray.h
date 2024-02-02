#pragma once

#include <Math/Vector3.h>

class Ray
{
public:
    Ray() = default;

    Ray(const Point3& origin, const Vector3& direction)
        : m_Origin(origin)
        , m_Direction(direction)
        , m_Time(0)
    {
    }

    Ray(const Point3& origin, const Vector3& direction, const double time)
        : m_Origin(origin)
        , m_Direction(direction)
        , m_Time(time)
    {
    }

    const Point3&  Origin() const;
    const Vector3& Direction() const;
    double         Time() const;
    Point3         At(double t) const;

private:
    Point3  m_Origin;
    Vector3 m_Direction;
    double  m_Time;
};
