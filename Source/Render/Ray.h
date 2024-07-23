#pragma once

#include <Math/Vector3.h>

namespace Render
{

class Ray
{
public:
    Ray() = default;

    Ray(const Math::Point3& origin, const Math::Vector3& direction)
        : m_Origin(origin)
        , m_Direction(direction)
        , m_Time(0)
    {
    }

    Ray(const Math::Point3& origin, const Math::Vector3& direction, const double time)
        : m_Origin(origin)
        , m_Direction(direction)
        , m_Time(time)
    {
    }

    const Math::Point3&  Origin() const;
    const Math::Vector3& Direction() const;
    double               Time() const;
    Math::Point3         At(double t) const;

private:
    Math::Point3  m_Origin;
    Math::Vector3 m_Direction;
    double        m_Time = 0;
};

}    // namespace Render
