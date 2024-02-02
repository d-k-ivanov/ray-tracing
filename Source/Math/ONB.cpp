#include "ONB.h"

#include "Vector3.h"

Vector3 ONB::operator[](const int i) const
{
    return m_Axis[i];
}

Vector3& ONB::operator[](const int i)
{
    return m_Axis[i];
}

Vector3 ONB::U() const
{
    return m_Axis[0];
}

Vector3 ONB::V() const
{
    return m_Axis[1];
}

Vector3 ONB::W() const
{
    return m_Axis[2];
}

Vector3 ONB::Local(const double a, const double b, const double c) const
{
    return a * U() + b * V() + c * W();
}

Vector3 ONB::Local(const Vector3& a) const
{
    return a.X() * U() + a.Y() * V() + a.Z() * W();
}

void ONB::BuildFromW(const Vector3& w)
{
    const Vector3 unitW = UnitVector(w);
    const Vector3 a     = (fabs(unitW.X()) > 0.9) ? Vector3(0, 1, 0) : Vector3(1, 0, 0);
    const Vector3 v     = UnitVector(CrossProduct(unitW, a));
    const Vector3 u     = CrossProduct(unitW, v);
    m_Axis[0]           = u;
    m_Axis[1]           = v;
    m_Axis[2]           = unitW;
}
