#pragma once

#include "Procedural.h"

#include <Utils/Glm.h>

namespace Objects
{

class Sphere final : public Procedural
{
public:
    Sphere(const glm::vec3& center, const float radius)
        : Center(center)
        , Radius(radius)
    {
    }

    const glm::vec3 Center;
    const float     Radius;

    std::pair<glm::vec3, glm::vec3> BoundingBox() const override
    {
        return std::make_pair(Center - Radius, Center + Radius);
    }
};

}    // namespace Objects
