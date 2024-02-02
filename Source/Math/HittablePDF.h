#pragma once

#include "PDF.h"

#include <Objects/Hittable.h>

class HittablePDF final : public PDF
{
public:
    HittablePDF(const Hittable& objects, const Point3& origin)
        : m_Objects(objects)
        , m_Origin(origin)
    {
    }

    double  Value(const Vector3& direction) const override;
    Vector3 Generate() const override;

private:
    const Hittable& m_Objects;
    Point3          m_Origin;
};
