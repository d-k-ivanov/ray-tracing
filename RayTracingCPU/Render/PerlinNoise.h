#pragma once

#include <Math/Vector3.h>

namespace Render
{

class Perlin
{
public:
    Perlin();
    ~Perlin() = default;

    double Noise(const Math::Point3& p) const;
    double Turbulence(const Math::Point3& p, int depth) const;

private:
    static constexpr int PointCount = 256;
    Math::Vector3        m_RandomVector[PointCount];
    int                  m_PermX[PointCount];
    int                  m_PermY[PointCount];
    int                  m_PermZ[PointCount];

    static void   PerlinGeneratePerm(int* p);
    static void   Permute(int* p, int n);
    static double PerlinInterp(const Math::Vector3 c[2][2][2], double u, double v, double w);
};

}    // namespace Render
