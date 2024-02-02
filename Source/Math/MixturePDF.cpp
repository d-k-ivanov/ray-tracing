#include "MixturePDF.h"

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
