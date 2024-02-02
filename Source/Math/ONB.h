#pragma once

#include "Vector3.h"

class ONB
{
public:
    ONB() = default;

    Vector3  operator[](int i) const;
    Vector3& operator[](int i);

    Vector3 U() const;
    Vector3 V() const;
    Vector3 W() const;

    Vector3 Local(double a, double b, double c) const;
    Vector3 Local(const Vector3& a) const;
    void    BuildFromW(const Vector3& w);

private:
    Vector3 m_Axis[3];
};
