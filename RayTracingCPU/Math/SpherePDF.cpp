#include "SpherePDF.h"

namespace Math
{

double SpherePDF::Value(const Vector3& direction) const
{
    return 1 / (4 * Pi);
}

Vector3 SpherePDF::Generate() const
{
    return RandomUnitVector();
}

}    // namespace Math
