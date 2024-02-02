#pragma once

#include <Math/Vector3.h>

class Perlin
{
public:
    Perlin();
    ~Perlin();

    double Noise(const Point3& p) const;
    double Turbulence(const Point3& p, int depth) const;

private:
    static constexpr int PointCount = 256;
    Vector3*             m_RandomVector;
    int*                 m_PermX;
    int*                 m_PermY;
    int*                 m_PermZ;

    static int*   PerlinGeneratePerm();
    static void   Permute(int* p, int n);
    static double PerlinInterp(const Vector3 c[2][2][2], double u, double v, double w);
};
