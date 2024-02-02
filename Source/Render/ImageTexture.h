#pragma once

#include "Texture.h"

#include <UI/Image.h>

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
