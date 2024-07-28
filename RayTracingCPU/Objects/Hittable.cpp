#include "Hittable.h"

#include <Math/Converters.h>

namespace Objects
{

double Hittable::PDFValue(const Math::Vector3& origin, const Math::Vector3& direction) const
{
    return 0.0;
}

Math::Vector3 Hittable::Random(const Math::Vector3& origin) const
{
    return {1, 0, 0};
}

}    // namespace Objects
