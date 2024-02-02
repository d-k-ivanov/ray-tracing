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

class CosinePDF final : public PDF
{
public:
    explicit CosinePDF(const Vector3& w);

    double  Value(const Vector3& direction) const override;
    Vector3 Generate() const override;

private:
    ONB m_Uvw;
};

class SpherePDF final : public PDF
{
public:
    SpherePDF() = default;

    double  Value(const Vector3& direction) const override;
    Vector3 Generate() const override;
};

class HittablePDF final : public PDF
{
public:
    HittablePDF(const Hittable& objects, const Point3& origin)
        : m_Objects(objects)
        , m_Origin(origin)
    {
    }

    double  Value(const Vector3& direction) const override;
    Vector3 Generate() const override;

private:
    const Hittable& m_Objects;
    Point3          m_Origin;
};

class MixturePDF final : public PDF
{
public:
    MixturePDF(const std::shared_ptr<PDF>& p0, const std::shared_ptr<PDF>& p1);

    double  Value(const Vector3& direction) const override;
    Vector3 Generate() const override;

private:
    std::shared_ptr<PDF> m_P[2];
};
