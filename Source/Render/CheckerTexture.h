#pragma once

#include "SolidColor.h"

#include <memory>

class CheckerTexture final : public Texture
{
public:
    CheckerTexture(const double scale, const std::shared_ptr<Texture>& even, const std::shared_ptr<Texture>& odd)
        : m_InvertedScale(1.0 / scale)
        , m_Even(even)
        , m_Odd(odd)
    {
    }

    CheckerTexture(const double scale, const Color3& color1, const Color3& color2)
        : m_InvertedScale(1.0 / scale)
        , m_Even(std::make_shared<SolidColor>(color1))
        , m_Odd(std::make_shared<SolidColor>(color2))
    {
    }

    Color3 Value(double u, double v, const Point3& point) const override;

private:
    double                   m_InvertedScale;
    std::shared_ptr<Texture> m_Even;
    std::shared_ptr<Texture> m_Odd;
};
