#include "PerlinNoise.h"

Perlin::Perlin()
{
    m_RandomVector = new Vector3[PointCount];
    for(int i = 0; i < PointCount; i++)
    {
        m_RandomVector[i] = UnitVector(Vector3::Random(-1, 1));
    }

    m_PermX = PerlinGeneratePerm();
    m_PermY = PerlinGeneratePerm();
    m_PermZ = PerlinGeneratePerm();
}

Perlin::~Perlin()
{
    delete[] m_RandomVector;
    delete[] m_PermX;
    delete[] m_PermY;
    delete[] m_PermZ;
}

double Perlin::Noise(const Point3& p) const
{
    const auto u = p.X() - floor(p.X());
    const auto v = p.Y() - floor(p.Y());
    const auto w = p.Z() - floor(p.Z());
    const auto i = static_cast<int>(floor(p.X()));
    const auto j = static_cast<int>(floor(p.Y()));
    const auto k = static_cast<int>(floor(p.Z()));

    Vector3 c[2][2][2];

    for(int di = 0; di < 2; di++)
    {
        for(int dj = 0; dj < 2; dj++)
        {
            for(int dk = 0; dk < 2; dk++)
            {
                c[di][dj][dk] = m_RandomVector[m_PermX[(i + di) & 255] ^ m_PermY[(j + dj) & 255] ^ m_PermZ[(k + dk) & 255]];
            }
        }
    }
    return PerlinInterp(c, u, v, w);
}

double Perlin::Turbulence(const Point3& p, const int depth) const
{
    auto accum  = 0.0;
    auto tempP  = p;
    auto weight = 1.0;

    for(int i = 0; i < depth; i++)
    {
        accum += weight * Noise(tempP);
        weight *= 0.5;
        tempP *= 2;
    }

    return fabs(accum);
}

int* Perlin::PerlinGeneratePerm()
{
    const auto p = new int[PointCount];

    for(int i = 0; i < PointCount; i++)
        p[i] = i;

    Permute(p, PointCount);

    return p;
}

void Perlin::Permute(int* p, const int n)
{
    for(int i = n - 1; i > 0; i--)
    {
        const int target = Random::Int(0, i);
        const int tmp    = p[i];
        p[i]             = p[target];
        p[target]        = tmp;
    }
}

double Perlin::PerlinInterp(const Vector3 c[2][2][2], const double u, const double v, const double w)
{
    const auto uu = u * u * (3 - 2 * u);
    const auto vv = v * v * (3 - 2 * v);
    const auto ww = w * w * (3 - 2 * w);

    auto accum = 0.0;

    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            for(int k = 0; k < 2; k++)
            {
                Vector3 weightV(u - i, v - j, w - k);
                accum += ((i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) * (k * ww + (1 - k) * (1 - ww)) * DotProduct(c[i][j][k], weightV));
            }
        }
    }
    return accum;
}
