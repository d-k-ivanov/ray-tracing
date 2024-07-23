#pragma once

#include "Color.h"

namespace Render
{

class Texture
{
public:
    virtual ~Texture() = default;

    virtual Color3 Value(double u, double v, const Math::Point3& point) const = 0;
};

}    // namespace Render
