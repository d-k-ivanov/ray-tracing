#pragma once

#include <Math/Vector3.h>
#include <Render/HittableList.h>
#include <Render/ONB.h>

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
    explicit CosinePDF(const Vector3& w) { m_Uvw.BuildFromW(w); }

    double Value(const Vector3& direction) const override
    {
        const auto cosineTheta = DotProduct(UnitVector(direction), m_Uvw.W());
        return fmax(0, cosineTheta / Pi);
    }

    Vector3 Generate() const override
    {
        return m_Uvw.Local(RandomCosineDirection());
    }

private:
    ONB m_Uvw;
};

class SpherePDF final : public PDF
{
public:
    SpherePDF() = default;

    double Value(const Vector3& direction) const override
    {
        return 1 / (4 * Pi);
    }

    Vector3 Generate() const override
    {
        return RandomUnitVector();
    }
};

class HittablePDF final : public PDF
{
public:
    HittablePDF(const Hittable& objects, const Point3& origin)
        : m_Objects(objects)
        , m_Origin(origin)
    {
    }

    double Value(const Vector3& direction) const override
    {
        return m_Objects.PDFValue(m_Origin, direction);
    }

    Vector3 Generate() const override
    {
        return m_Objects.Random(m_Origin);
    }

private:
    const Hittable& m_Objects;
    Point3          m_Origin;
};

class MixturePDF final : public PDF
{
public:
    MixturePDF(const std::shared_ptr<PDF>& p0, const std::shared_ptr<PDF>& p1)
    {
        m_P[0] = p0;
        m_P[1] = p1;
    }

    double Value(const Vector3& direction) const override
    {
        return 0.5 * m_P[0]->Value(direction) + 0.5 * m_P[1]->Value(direction);
    }

    Vector3 Generate() const override
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

private:
    std::shared_ptr<PDF> m_P[2];
};
