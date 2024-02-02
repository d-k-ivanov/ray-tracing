#pragma once

#include "PDF.h"

#include <memory>

class MixturePDF final : public PDF
{
public:
    MixturePDF(const std::shared_ptr<PDF>& p0, const std::shared_ptr<PDF>& p1);

    double  Value(const Vector3& direction) const override;
    Vector3 Generate() const override;

private:
    std::shared_ptr<PDF> m_P[2];
};
