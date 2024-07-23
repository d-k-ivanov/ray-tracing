#pragma once

#include "PerlinNoise.h"
#include "Texture.h"

namespace Render
{

class NoiseTextureSmooth final : public Texture
{
public:
    NoiseTextureSmooth() = default;

    explicit NoiseTextureSmooth(const double scale)
        : m_Scale(scale)
    {
    }

    Color3 Value(double u, double v, const Math::Point3& p) const override;

private:
    Perlin m_Noise;
    double m_Scale;
};

}    // namespace Render
