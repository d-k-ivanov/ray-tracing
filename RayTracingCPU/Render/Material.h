#pragma once

#include "Color.h"

#include <memory>

namespace Math
{
class PDF;
}    // namespace Math

namespace Render
{

class HitRecord;
class Ray;
class Texture;
class ScatterRecord;

class Material
{
public:
    virtual ~Material() = default;

    virtual Color3 Emitted(double u, double v, const Math::Point3& p) const;
    virtual Color3 Emitted(const Ray& rIn, const HitRecord& rec, double u, double v, const Math::Point3& p) const;

    virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const              = 0;
    virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const = 0;
    virtual bool Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const;

    virtual double ScatteringPDF(const Ray& rIn, const HitRecord& rec, const Ray& scattered) const;
};

}    // namespace Render
