#include "PDF.h"

#include "ONB.h"
#include "Vector3.h"

#include <Objects/Hittable.h>

CosinePDF::CosinePDF(const Vector3& w)
{
    m_Uvw.BuildFromW(w);
}

double CosinePDF::Value(const Vector3& direction) const
{
    const auto cosineTheta = DotProduct(UnitVector(direction), m_Uvw.W());
    return fmax(0, cosineTheta / Pi);
}

Vector3 CosinePDF::Generate() const
{
    return m_Uvw.Local(RandomCosineDirection());
}

double SpherePDF::Value(const Vector3& direction) const
{
    return 1 / (4 * Pi);
}

Vector3 SpherePDF::Generate() const
{
    return RandomUnitVector();
}

double HittablePDF::Value(const Vector3& direction) const
{
    return m_Objects.PDFValue(m_Origin, direction);
}

Vector3 HittablePDF::Generate() const
{
    return m_Objects.Random(m_Origin);
}

MixturePDF::MixturePDF(const std::shared_ptr<PDF>& p0, const std::shared_ptr<PDF>& p1)
{
    m_P[0] = p0;
    m_P[1] = p1;
}

double MixturePDF::Value(const Vector3& direction) const
{
    return 0.5 * m_P[0]->Value(direction) + 0.5 * m_P[1]->Value(direction);
}

Vector3 MixturePDF::Generate() const
{
    if(Random::Double() < 0.5)
    {
        return m_P[0]->Generate();
    }
    else
    {
        return m_P[1]->Generate();
    }
}
