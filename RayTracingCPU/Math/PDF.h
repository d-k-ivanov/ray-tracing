#pragma once

#include "Vector3.h"

namespace Math
{

class PDF
{
public:
    virtual ~PDF() = default;

    virtual double  Value(const Vector3& direction) const = 0;
    virtual Vector3 Generate() const                      = 0;
};

}    // namespace Math
