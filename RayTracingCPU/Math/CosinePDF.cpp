#include "CosinePDF.h"

#include "Converters.h"

namespace Math
{

CosinePDF::CosinePDF(const Vector3& w)
{
    m_Uvw.BuildFromW(w);
}

double CosinePDF::Value(const Vector3& direction) const
{
    const auto cosineTheta = DotProduct(UnitVector(direction), m_Uvw.W());
    // return 1 / (2 * Pi) * (1 + cosineTheta);
    return fmax(0, cosineTheta / Pi);
}

Vector3 CosinePDF::Generate() const
{
    return m_Uvw.Local(RandomCosineDirection());
}

}    // namespace Math
