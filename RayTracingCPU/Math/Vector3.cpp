#include "Vector3.h"

namespace Math
{

double Vector3::X() const
{
    return E[0];
}

double Vector3::Y() const
{
    return E[1];
}

double Vector3::Z() const
{
    return E[2];
}

Vector3 Vector3::operator-(const Vector3& other) const
{
    return {E[0] - other.E[0],
            E[1] - other.E[1],
            E[2] - other.E[2]};
}

void Vector3::operator-=(const Vector3& other)
{
    E[0] -= other.E[0];
    E[1] -= other.E[1];
    E[2] -= other.E[2];
}

Vector3 Vector3::operator-() const
{
    return {-E[0], -E[1], -E[2]};
}

double Vector3::operator[](const int i) const
{
    return E[i];
}

double& Vector3::operator[](const int i)
{
    return E[i];
}

Vector3& Vector3::operator+=(const Vector3& v)
{
    E[0] += v.E[0];
    E[1] += v.E[1];
    E[2] += v.E[2];
    return *this;
}

Vector3& Vector3::operator*=(const double t)
{
    E[0] *= t;
    E[1] *= t;
    E[2] *= t;
    return *this;
}

Vector3& Vector3::operator/=(const double t)
{
    return *this *= 1 / t;
}

double Vector3::Length() const
{
    return sqrt(LengthSquared());
}

double Vector3::LengthSquared() const
{
    return E[0] * E[0] + E[1] * E[1] + E[2] * E[2];
}

// Return true if the vector is close to zero in all dimensions.
bool Vector3::NearZero() const
{
    constexpr auto tolerance = 1e-8;
    return (fabs(E[0]) < tolerance) && (fabs(E[1]) < tolerance) && (fabs(E[2]) < tolerance);
}

Vector3 Vector3::Random()
{
    return {Utils::Random::Double(), Utils::Random::Double(), Utils::Random::Double()};
}

Vector3 Vector3::Random(const double min, const double max)
{
    return {Utils::Random::Double(min, max), Utils::Random::Double(min, max), Utils::Random::Double(min, max)};
}

}    // namespace Math
