#include "ImageTexture.h"

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
