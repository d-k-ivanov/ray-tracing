#include "Hittable.h"

#include <Math/Converters.h>

double Hittable::PDFValue(const Vector3& origin, const Vector3& direction) const
{
    return 0.0;
}

Vector3 Hittable::Random(const Vector3& origin) const
{
    return {1, 0, 0};
}
