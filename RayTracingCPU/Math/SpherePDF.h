#pragma once

#include "PDF.h"

namespace Math
{

class SpherePDF final : public PDF
{
public:
    SpherePDF() = default;

    double  Value(const Vector3& direction) const override;
    Vector3 Generate() const override;
};

}    // namespace Math
