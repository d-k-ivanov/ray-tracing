// ReSharper disable CppClangTidyModernizeUseNodiscard
#pragma once

#include <Math/Constants.h>
#include <Utils/Random.h>

#include <cmath>
#include <iostream>

class Vector3
{
public:
    double e[3];

    Vector3()
        : e{0, 0, 0}
    {
    }
    Vector3(const double e0, const double e1, const double e2)
        : e{e0, e1, e2}
    {
    }

    double x() const
    {
        return e[0];
    }

    double y() const
    {
        return e[1];
    }

    double z() const
    {
        return e[2];
    }

    Vector3 operator-(const Vector3& other) const
    {
        return {e[0] - other.e[0],
            e[1] - other.e[1],
            e[2] - other.e[2]};
    }

    void operator-=(const Vector3& other)
    {
        e[0] -= other.e[0];
        e[1] -= other.e[1];
        e[2] -= other.e[2];
    }

    Vector3 operator-() const
    {
        return {-e[0], -e[1], -e[2]};
    }

    double operator[](const int i) const
    {
        return e[i];
    }

    double& operator[](const int i)
    {
        return e[i];
    }

    Vector3& operator+=(const Vector3& v)
    {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    Vector3& operator*=(const double t)
    {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Vector3& operator/=(const double t)
    {
        return *this *= 1 / t;
    }

    double Length() const
    {
        return sqrt(LengthSquared());
    }

    double LengthSquared() const
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    // Return true if the vector is close to zero in all dimensions.
    bool NearZero() const
    {
        constexpr auto tolerance = 1e-8;
        return (fabs(e[0]) < tolerance) && (fabs(e[1]) < tolerance) && (fabs(e[2]) < tolerance);
    }

    static Vector3 Random()
    {
        return {Random::Double(), Random::Double(), Random::Double()};
    }

    static Vector3 Random(const double min, const double max)
    {
        return {Random::Double(min, max), Random::Double(min, max), Random::Double(min, max)};
    }
};

using Point3 = Vector3;

inline std::ostream& operator<<(std::ostream& out, const Vector3& v)
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline Vector3 operator+(const Vector3& u, const Vector3& v)
{
    return {u.e[0] + v.e[0],
        u.e[1] + v.e[1],
        u.e[2] + v.e[2]};
}

// inline Vector3 operator-(const Vector3 &u, const Vector3 &v)
// {
//     return {u.e[0] - v.e[0],
//             u.e[1] - v.e[1],
//             u.e[2] - v.e[2]};
// }

inline Vector3 operator*(const Vector3& u, const Vector3& v)
{
    return {u.e[0] * v.e[0],
        u.e[1] * v.e[1],
        u.e[2] * v.e[2]};
}

inline Vector3 operator*(const double t, const Vector3& v)
{
    return {t * v.e[0],
        t * v.e[1],
        t * v.e[2]};
}

inline Vector3 operator*(const Vector3& v, double t)
{
    return t * v;
}

inline Vector3 operator/(const Vector3& v, const double t)
{
    return (1 / t) * v;
}

inline double Dot(const Vector3& u, const Vector3& v)
{
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline Vector3 Cross(const Vector3& u, const Vector3& v)
{
    return {u.e[1] * v.e[2] - u.e[2] * v.e[1],
        u.e[2] * v.e[0] - u.e[0] * v.e[2],
        u.e[0] * v.e[1] - u.e[1] * v.e[0]};
}

inline Vector3 UnitVector(const Vector3& v)
{
    return v / v.Length();
}

inline Vector3 RandomInUnitDisk()
{
    while(true)
    {
        auto p = Vector3(Random::Double(-1, 1), Random::Double(-1, 1), 0);
        if(p.LengthSquared() < 1)
        {
            return p;
        }
    }
}

inline Vector3 RandomInUnitSphere()
{
    while(true)
    {
        auto p = Vector3::Random(-1, 1);
        if(p.LengthSquared() < 1)
        {
            return p;
        }
    }
}

inline Vector3 RandomUnitVector()
{
    return UnitVector(RandomInUnitSphere());
}

inline Vector3 random_on_hemisphere(const Vector3& normal)
{
    const Vector3 on_unit_sphere = RandomUnitVector();
    if(Dot(on_unit_sphere, normal) > 0.0)    // In the same hemisphere as the normal
    {
        return on_unit_sphere;
    }
    else
    {
        return -on_unit_sphere;
    }
}

inline Vector3 Reflect(const Vector3& v, const Vector3& n)
{
    return v - 2 * Dot(v, n) * n;
}

inline Vector3 Refract(const Vector3& uv, const Vector3& n, double etai_over_etat)
{
    const auto    cosTheta     = fmin(Dot(-uv, n), 1.0);
    const Vector3 rOutPerp     = etai_over_etat * (uv + cosTheta * n);
    const Vector3 rOutParallel = -sqrt(fabs(1.0 - rOutPerp.LengthSquared())) * n;
    return rOutPerp + rOutParallel;
}

inline Vector3 RandomCosineDirection()
{
    const auto r1 = Random::Double();
    const auto r2 = Random::Double();

    const auto phi = 2 * Pi * r1;
    const auto x   = cos(phi) * sqrt(r2);
    const auto y   = sin(phi) * sqrt(r2);
    const auto z   = sqrt(1 - r2);

    return {x, y, z};
}
