#pragma once

#include <Math/Constants.h>
#include <Utils/Random.h>

#include <cmath>
#include <iostream>

class Vector3
{
public:
    double E[3];

    Vector3()
        : E {0, 0, 0}
    {
    }

    Vector3(const double e0, const double e1, const double e2)
        : E {e0, e1, e2}
    {
    }

    double X() const;
    double Y() const;
    double Z() const;

    Vector3  operator-(const Vector3& other) const;
    void     operator-=(const Vector3& other);
    Vector3  operator-() const;
    double   operator[](int i) const;
    double&  operator[](int i);
    Vector3& operator+=(const Vector3& v);
    Vector3& operator*=(double t);
    Vector3& operator/=(double t);

    double Length() const;
    double LengthSquared() const;

    // Return true if the vector is close to zero in all dimensions.
    bool NearZero() const;

    static Vector3 Random();
    static Vector3 Random(double min, double max);
};

using Point3 = Vector3;

inline std::ostream& operator<<(std::ostream& out, const Vector3& v)
{
    return out << v.E[0] << ' ' << v.E[1] << ' ' << v.E[2];
}

inline Vector3 operator+(const Vector3& u, const Vector3& v)
{
    return {u.E[0] + v.E[0],
            u.E[1] + v.E[1],
            u.E[2] + v.E[2]};
}

// inline Vector3 operator-(const Vector3 &u, const Vector3 &v)
// {
//     return {u.E[0] - v.E[0],
//             u.E[1] - v.E[1],
//             u.E[2] - v.E[2]};
// }

inline Vector3 operator*(const Vector3& u, const Vector3& v)
{
    return {u.E[0] * v.E[0],
            u.E[1] * v.E[1],
            u.E[2] * v.E[2]};
}

inline Vector3 operator*(const double t, const Vector3& v)
{
    return {t * v.E[0],
            t * v.E[1],
            t * v.E[2]};
}

inline Vector3 operator*(const Vector3& v, const double t)
{
    return t * v;
}

inline Vector3 operator/(const Vector3& v, const double t)
{
    return (1 / t) * v;
}

inline double DotProduct(const Vector3& u, const Vector3& v)
{
    return u.E[0] * v.E[0] + u.E[1] * v.E[1] + u.E[2] * v.E[2];
}

inline Vector3 CrossProduct(const Vector3& u, const Vector3& v)
{
    return {u.E[1] * v.E[2] - u.E[2] * v.E[1],
            u.E[2] * v.E[0] - u.E[0] * v.E[2],
            u.E[0] * v.E[1] - u.E[1] * v.E[0]};
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
