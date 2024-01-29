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

    const Point3& Origin() const
    {
        return m_Origin;
    }

    const Vector3& Direction() const
    {
        return m_Direction;
    }

    double Time() const
    {
        return m_Time;
    }

    Point3 At(const double t) const
    {
        return m_Origin + t * m_Direction;
    }

private:
    Point3  m_Origin;
    Vector3 m_Direction;
    double  m_Time;
};
