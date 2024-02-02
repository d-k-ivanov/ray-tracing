#include "HittablePDF.h"

double HittablePDF::Value(const Vector3& direction) const
{
    return m_Objects.PDFValue(m_Origin, direction);
}

Vector3 HittablePDF::Generate() const
{
    return m_Objects.Random(m_Origin);
}
