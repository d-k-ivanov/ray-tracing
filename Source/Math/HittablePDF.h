#pragma once

#include "PDF.h"

#include <Objects/Hittable.h>

namespace Math
{

class HittablePDF final : public PDF
{
public:
    HittablePDF(const Objects::Hittable& objects, const Point3& origin)
        : m_Objects(objects)
        , m_Origin(origin)
    {
    }

    double  Value(const Vector3& direction) const override;
    Vector3 Generate() const override;

private:
    const Objects::Hittable& m_Objects;
    Point3                   m_Origin;
};

}    // namespace Math
