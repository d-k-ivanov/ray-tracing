#pragma once

#include "Texture.h"
#include "PerlinNoise.h"

class NoiseTextureCamouflage final : public Texture
{
public:
    NoiseTextureCamouflage() = default;

    explicit NoiseTextureCamouflage(const double scale)
        : m_Scale(scale)
    {
    }

    Color3 Value(double u, double v, const Point3& p) const override;

private:
    Perlin m_Noise;
    double m_Scale;
};