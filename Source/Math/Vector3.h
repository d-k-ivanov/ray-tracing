// ReSharper disable CppClangTidyModernizeUseNodiscard
#pragma once

#include <Math/Constants.h>
#include <Utils/Random.h>

#include <cmath>
#include <iostream>

class Vector3
{
public:
    double V[3];

    Vector3()
        : V{0, 0, 0}
    {
    }
    Vector3(const double e0, const double e1, const double e2)
        : V{e0, e1, e2}
    {
    }

    double X() const
    {
        return V[0];
    }

    double Y() const
    {
        return V[1];
    }

    double Z() const
    {
        return V[2];
    }

    Vector3 operator-(const Vector3& other) const
    {
        return {V[0] - other.V[0],
                V[1] - other.V[1],
                V[2] - other.V[2]};
    }

    void operator-=(const Vector3& other)
    {
        V[0] -= other.V[0];
        V[1] -= other.V[1];
        V[2] -= other.V[2];
    }

    Vector3 operator-() const
    {
        return {-V[0], -V[1], -V[2]};
    }

    double operator[](const int i) const
    {
        return V[i];
    }

    double& operator[](const int i)
    {
        return V[i];
    }

    Vector3& operator+=(const Vector3& v)
    {
        V[0] += v.V[0];
        V[1] += v.V[1];
        V[2] += v.V[2];
        return *this;
    }

    Vector3& operator*=(const double t)
    {
        V[0] *= t;
        V[1] *= t;
        V[2] *= t;
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
        return V[0] * V[0] + V[1] * V[1] + V[2] * V[2];
    }

    // Return true if the vector is close to zero in all dimensions.
    bool NearZero() const
    {
        constexpr auto tolerance = 1e-8;
        return (fabs(V[0]) < tolerance) && (fabs(V[1]) < tolerance) && (fabs(V[2]) < tolerance);
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
    return out << v.V[0] << ' ' << v.V[1] << ' ' << v.V[2];
}

inline Vector3 operator+(const Vector3& u, const Vector3& v)
{
    return {u.V[0] + v.V[0],
            u.V[1] + v.V[1],
            u.V[2] + v.V[2]};
}

// inline Vector3 operator-(const Vector3 &u, const Vector3 &v)
// {
//     return {u.V[0] - v.V[0],
//             u.V[1] - v.V[1],
//             u.V[2] - v.V[2]};
// }

inline Vector3 operator*(const Vector3& u, const Vector3& v)
{
    return {u.V[0] * v.V[0],
            u.V[1] * v.V[1],
            u.V[2] * v.V[2]};
}

inline Vector3 operator*(const double t, const Vector3& v)
{
    return {t * v.V[0],
            t * v.V[1],
            t * v.V[2]};
}

inline Vector3 operator*(const Vector3& v, double t)
{
    return t * v;
}

inline Vector3 operator/(const Vector3& v, const double t)
{
    return (1 / t) * v;
}

inline double DotProduct(const Vector3& u, const Vector3& v)
{
    return u.V[0] * v.V[0] + u.V[1] * v.V[1] + u.V[2] * v.V[2];
}

inline Vector3 CrossProduct(const Vector3& u, const Vector3& v)
{
    return {u.V[1] * v.V[2] - u.V[2] * v.V[1],
            u.V[2] * v.V[0] - u.V[0] * v.V[2],
            u.V[0] * v.V[1] - u.V[1] * v.V[0]};
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

inline Vector3 RandomOnHemisphere(const Vector3& normal)
{
    const Vector3 onUnitSphere = RandomUnitVector();
    if(DotProduct(onUnitSphere, normal) > 0.0)    // In the same hemisphere as the normal
    {
        return onUnitSphere;
    }
    else
    {
        return -onUnitSphere;
    }
}

inline Vector3 Reflect(const Vector3& v, const Vector3& n)
{
    return v - 2 * DotProduct(v, n) * n;
}

inline Vector3 Refract(const Vector3& uv, const Vector3& n, const double etaIOverEtaT)
{
    const auto    cosTheta     = fmin(DotProduct(-uv, n), 1.0);
    const Vector3 rOutPerp     = etaIOverEtaT * (uv + cosTheta * n);
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
