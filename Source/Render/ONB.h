#pragma once

class ONB
{
public:
    ONB() = default;

    Vector3  operator[](const int i) const { return m_Axis[i]; }
    Vector3& operator[](const int i) { return m_Axis[i]; }

    Vector3 U() const { return m_Axis[0]; }
    Vector3 V() const { return m_Axis[1]; }
    Vector3 W() const { return m_Axis[2]; }

    Vector3 Local(const double a, const double b, const double c) const
    {
        return a * U() + b * V() + c * W();
    }

    Vector3 Local(const Vector3& a) const
    {
        return a.X() * U() + a.Y() * V() + a.Z() * W();
    }

    void BuildFromW(const Vector3& w)
    {
        const Vector3 unit_w = UnitVector(w);
        const Vector3 a      = (fabs(unit_w.X()) > 0.9) ? Vector3(0, 1, 0) : Vector3(1, 0, 0);
        const Vector3 v      = UnitVector(CrossProduct(unit_w, a));
        const Vector3 u      = CrossProduct(unit_w, v);
        m_Axis[0]            = u;
        m_Axis[1]            = v;
        m_Axis[2]            = unit_w;
    }

private:
    Vector3 m_Axis[3];
};
