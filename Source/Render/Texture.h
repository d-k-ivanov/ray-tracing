#pragma once

#include "Color.h"
#include "PerlinNoise.h"

#include <Math/Interval.h>
#include <Math/Vector3.h>
#include <UI/Image.h>

#include <memory>

class Texture
{
public:
    virtual ~Texture() = default;

    virtual Color3 Value(double u, double v, const Point3& point) const = 0;
};

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

    Color3 Value(double u, double v, const Point3& point) const override
    {
        return m_ColorValue;
    }

private:
    Color3 m_ColorValue;
};

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

    Color3 Value(const double u, const double v, const Point3& point) const override
    {
        const auto xInteger = static_cast<int>(std::floor(m_InvertedScale * point.X()));
        const auto yInteger = static_cast<int>(std::floor(m_InvertedScale * point.Y()));
        const auto zInteger = static_cast<int>(std::floor(m_InvertedScale * point.Z()));

        const bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? m_Even->Value(u, v, point) : m_Odd->Value(u, v, point);
    }

private:
    double                   m_InvertedScale;
    std::shared_ptr<Texture> m_Even;
    std::shared_ptr<Texture> m_Odd;
};

class NoiseTextureSmooth final : public Texture
{
public:
    NoiseTextureSmooth() = default;

    explicit NoiseTextureSmooth(const double scale)
        : m_Scale(scale)
    {
    }

    Color3 Value(double u, double v, const Point3& p) const override
    {
        return Color3(1, 1, 1) * 0.5 * (1.0 + m_Noise.Noise(m_Scale * p));
    }

private:
    Perlin m_Noise;
    double m_Scale;
};

class NoiseTextureCamouflage final : public Texture
{
public:
    NoiseTextureCamouflage() = default;

    explicit NoiseTextureCamouflage(const double scale)
        : m_Scale(scale)
    {
    }

    Color3 Value(double u, double v, const Point3& p) const override
    {
        const auto s = m_Scale * p;
        return Color3(1, 1, 1) * m_Noise.Turbulence(s, 7);
    }

private:
    Perlin m_Noise;
    double m_Scale;
};

class NoiseTexture final : public Texture
{
public:
    NoiseTexture() = default;

    explicit NoiseTexture(const double scale)
        : m_Scale(scale)
    {
    }

    Color3 Value(double u, double v, const Point3& p) const override
    {
        const auto s = m_Scale * p;
        return Color3(1, 1, 1) * 0.5 * (1 + sin(s.Z() + 10 * m_Noise.Turbulence(s, 7)));
    }

private:
    Perlin m_Noise;
    double m_Scale;
};

class ImageTexture final : public Texture
{
public:
    explicit ImageTexture(const std::string_view path)
        : m_Image(path, true)
    {
    }

    Color3 Value(double u, double v, const Point3& p) const override
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

private:
    // rtw_image image;
    Image m_Image;
};
