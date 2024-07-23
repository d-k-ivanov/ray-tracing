#pragma once

#include "ONB.h"
#include "PDF.h"

namespace Math
{

class CosinePDF final : public PDF
{
public:
    explicit CosinePDF(const Vector3& w);

    double  Value(const Vector3& direction) const override;
    Vector3 Generate() const override;

private:
    ONB m_Uvw;
};

}    // namespace Math
