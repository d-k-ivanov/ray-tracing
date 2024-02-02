#include "Texture.h"

#include <Math/Interval.h>

#include <memory>

Color3 SolidColor::Value(double u, double v, const Point3& point) const
{
    return m_ColorValue;
}

Color3 CheckerTexture::Value(const double u, const double v, const Point3& point) const
{
    const auto xInteger = static_cast<int>(std::floor(m_InvertedScale * point.X()));
    const auto yInteger = static_cast<int>(std::floor(m_InvertedScale * point.Y()));
    const auto zInteger = static_cast<int>(std::floor(m_InvertedScale * point.Z()));

    const bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

    return isEven ? m_Even->Value(u, v, point) : m_Odd->Value(u, v, point);
}

Color3 NoiseTextureSmooth::Value(double u, double v, const Point3& p) const
{
    return Color3(1, 1, 1) * 0.5 * (1.0 + m_Noise.Noise(m_Scale * p));
}

Color3 NoiseTextureCamouflage::Value(double u, double v, const Point3& p) const
{
    const auto s = m_Scale * p;
    return Color3(1, 1, 1) * m_Noise.Turbulence(s, 7);
}

Color3 NoiseTexture::Value(double u, double v, const Point3& p) const
{
    const auto s = m_Scale * p;
    return Color3(1, 1, 1) * 0.5 * (1 + sin(s.Z() + 10 * m_Noise.Turbulence(s, 7)));
}

Color3 ImageTexture::Value(double u, double v, const Point3& p) const
{
    // If we have no texture data, then return solid cyan as a debugging aid.
    if(m_Image.GetHeight() <= 0)
        return {0, 1, 1};

    // Clamp input texture coordinates to [0,1] x [1,0]
    u = Interval(0, 1).Clamp(u);
    // Flip V to image coordinates
    v = 1.0 - Interval(0, 1).Clamp(v);

    const auto i     = static_cast<int>(u * m_Image.GetWidth());
    const auto j     = static_cast<int>(v * m_Image.GetHeight());
    const auto pixel = m_Image.PixelData(i, j);

    constexpr auto colorScale = 1.0 / 255.0;
    return {colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]};
}
