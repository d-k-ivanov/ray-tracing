#pragma once

#include <Utils/Glm.h>

#include <utility>

namespace Objects
{
class Procedural
{
public:
    Procedural()          = default;
    virtual ~Procedural() = default;

    Procedural(const Procedural&)            = delete;
    Procedural(Procedural&&)                 = delete;
    Procedural& operator=(const Procedural&) = delete;
    Procedural& operator=(Procedural&&)      = delete;

    virtual std::pair<glm::vec3, glm::vec3> BoundingBox() const = 0;
};

}    // namespace Objects
