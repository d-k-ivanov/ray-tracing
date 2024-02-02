#pragma once

#include "Texture.h"

class SolidColor final : public Texture
{
public:
    explicit SolidColor(const Color3& color)
        : m_ColorValue(color)
    {
    }

    SolidColor(const double red, const double green, const double blue)
        : SolidColor(Color3(red, green, blue))
    {
    }

    Color3 Value(double u, double v, const Point3& point) const override;

private:
    Color3 m_ColorValue;
};
