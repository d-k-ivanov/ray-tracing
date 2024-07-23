#pragma once

#include "Quad.h"

namespace Objects
{

// Returns the 3D box (six sides) that contains the two opposite vertices a & b.
inline std::shared_ptr<HittableList> Box(const Math::Point3& a, const Math::Point3& b, const std::shared_ptr<Render::Material>& material)
{
    auto sides = std::make_shared<HittableList>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    const auto min = Math::Point3(fmin(a.X(), b.X()), fmin(a.Y(), b.Y()), fmin(a.Z(), b.Z()));
    const auto max = Math::Point3(fmax(a.X(), b.X()), fmax(a.Y(), b.Y()), fmax(a.Z(), b.Z()));

    auto dx = Math::Vector3(max.X() - min.X(), 0, 0);
    auto dy = Math::Vector3(0, max.Y() - min.Y(), 0);
    auto dz = Math::Vector3(0, 0, max.Z() - min.Z());

    sides->Add(std::make_shared<Quad>(Math::Point3(min.X(), min.Y(), max.Z()), dx, dy, material));     // front
    sides->Add(std::make_shared<Quad>(Math::Point3(max.X(), min.Y(), max.Z()), -dz, dy, material));    // right
    sides->Add(std::make_shared<Quad>(Math::Point3(max.X(), min.Y(), min.Z()), -dx, dy, material));    // back
    sides->Add(std::make_shared<Quad>(Math::Point3(min.X(), min.Y(), min.Z()), dz, dy, material));     // left
    sides->Add(std::make_shared<Quad>(Math::Point3(min.X(), max.Y(), max.Z()), dx, -dz, material));    // top
    sides->Add(std::make_shared<Quad>(Math::Point3(min.X(), min.Y(), min.Z()), dx, dz, material));     // bottom

    return sides;
}

}    // namespace Objects
