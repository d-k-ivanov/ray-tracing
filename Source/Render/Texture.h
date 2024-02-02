#pragma once

#include "Color.h"
#include "PerlinNoise.h"

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

    Color3 Value(double u, double v, const Point3& point) const override;

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

    Color3 Value(double u, double v, const Point3& point) const override;

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

    Color3 Value(double u, double v, const Point3& p) const override;

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

    Color3 Value(double u, double v, const Point3& p) const override;

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

    Color3 Value(double u, double v, const Point3& p) const override;

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

    Color3 Value(double u, double v, const Point3& p) const override;

private:
    Image m_Image;
};
