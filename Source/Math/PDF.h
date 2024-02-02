#pragma once

#include "ONB.h"
#include "Vector3.h"

#include <Objects/Hittable.h>

class PDF
{
public:
    virtual ~PDF() = default;

    virtual double  Value(const Vector3& direction) const = 0;
    virtual Vector3 Generate() const                      = 0;
};
