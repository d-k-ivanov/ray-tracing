#pragma once

#include "Texture.h"

#include <Engine/Image.h>

namespace Render
{

class ImageTexture final : public Texture
{
public:
    explicit ImageTexture(const std::string_view path)
        : m_Image(path, true)
    {
    }

    Color3 Value(double u, double v, const Math::Point3& p) const override;

private:
    Engine::Image m_Image;
};

}    // namespace Render
