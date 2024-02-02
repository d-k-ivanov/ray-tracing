#pragma once

#include "Color.h"

class Texture
{
public:
    virtual ~Texture() = default;

    virtual Color3 Value(double u, double v, const Point3& point) const = 0;
};
