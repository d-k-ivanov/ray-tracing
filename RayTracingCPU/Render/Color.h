#pragma once

#include <Math/Interval.h>
#include <Math/Vector3.h>

#include <cmath>

namespace Render
{

using Color3 = Math::Vector3;

inline double LinearToGamma(const double linearComponent)
{
    if(linearComponent > 0)
    {
        return sqrt(linearComponent);
    }
    return 0.0;
}

// Write the translated [0,255] value of each color component.
inline uint32_t GetColorRGBA(const Color3& color, const double colorScale)
{
    auto   red   = color.X();
    auto   green = color.Y();
    auto   blue  = color.Z();
    double alpha = 1.0;

    // Replace NaN components with zero.
    // This seems to be not working...
    if(std::isnan(red))
    {
        red = 0.0;
    }

    if(std::isnan(green))
    {
        green = 0.0;
    }

    if(std::isnan(blue))
    {
        blue = 0.0;
    }

    if(std::isnan(alpha))
    {
        alpha = 1.0;
    }

    // Divide the color by the number of samples.
    red *= colorScale;
    green *= colorScale;
    blue *= colorScale;

    // Apply the linear to gamma transform (gamma correction).
    red   = LinearToGamma(red);
    green = LinearToGamma(green);
    blue  = LinearToGamma(blue);

    static const Math::Interval Intensity(0.000, 0.999);

    const uint8_t r = static_cast<uint8_t>(255.999 * Intensity.Clamp(red));
    const uint8_t g = static_cast<uint8_t>(255.999 * Intensity.Clamp(green));
    const uint8_t b = static_cast<uint8_t>(255.999 * Intensity.Clamp(blue));
    const uint8_t a = static_cast<uint8_t>(255.999 * Intensity.Clamp(alpha));

    return (a << 24) | (b << 16) | (g << 8) | r;
}

inline uint32_t GetColorRGBANoGammaCorrection(const Color3& color, const double colorScale)
{
    auto   red   = color.X();
    auto   green = color.Y();
    auto   blue  = color.Z();
    double alpha = 1.0;

    // Replace NaN components with zero.
    if(std::isnan(red))
    {
        red = 0.0;
    }

    if(std::isnan(green))
    {
        green = 0.0;
    }

    if(std::isnan(blue))
    {
        blue = 0.0;
    }

    if(std::isnan(alpha))
    {
        alpha = 0.0;
    }

    // Divide the color by the number of samples.
    red *= colorScale;
    green *= colorScale;
    blue *= colorScale;

    static const Math::Interval Intensity(0.000, 0.999);

    const uint8_t r = static_cast<uint8_t>(255.999 * Intensity.Clamp(red));
    const uint8_t g = static_cast<uint8_t>(255.999 * Intensity.Clamp(green));
    const uint8_t b = static_cast<uint8_t>(255.999 * Intensity.Clamp(blue));
    const uint8_t a = static_cast<uint8_t>(255.999 * Intensity.Clamp(alpha));

    return (a << 24) | (b << 16) | (g << 8) | r;
}

inline uint32_t GetRandomColorRGBA()
{
    return Utils::Random::UInt() | 0xff000000;
}

}    // namespace Render
