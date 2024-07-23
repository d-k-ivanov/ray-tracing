#pragma once

#include "PerlinNoise.h"
#include "Texture.h"

namespace Render
{

class NoiseTexture final : public Texture
{
public:
    NoiseTexture() = default;

    explicit NoiseTexture(const double scale)
        : m_Scale(scale)
    {
    }

    Color3 Value(double u, double v, const Math::Point3& p) const override;

private:
    Perlin m_Noise;
    double m_Scale;
};

}    // namespace Render
