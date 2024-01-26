// ReSharper disable CppNonExplicitConvertingConstructor
#pragma once

#include <Render/Color.h>
#include <Render/Hittable.h>
#include <Render/Ray.h>

class HitRecord;

class Material
{
public:
    virtual ~Material() = default;

    virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const = 0;
};

class Lambertian final : public Material
{
public:
    Lambertian(const Color3& color)
        : m_Albedo(color)
    {
    }

    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const override
    {
        auto scatterDirection = rec.Normal + RandomUnitVector();

        // Catch degenerate scatter direction
        if(scatterDirection.NearZero())
        {
            scatterDirection = rec.Normal;
        }
        scattered   = Ray(rec.P, scatterDirection, rIn.Time());
        attenuation = m_Albedo;
        return true;
    }

private:
    Color3 m_Albedo;
};

class Metal final : public Material
{
public:
    Metal(const Color3& color, const double fuzz)
        : m_Albedo(color)
        , m_Fuzz(fuzz < 1 ? fuzz : 1)
    {
    }

    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const override
    {
        const Vector3 reflected = Reflect(UnitVector(rIn.Direction()), rec.Normal);
        scattered               = Ray(rec.P, reflected + m_Fuzz * RandomUnitVector(), rIn.Time());
        attenuation             = m_Albedo;
        return (DotProduct(scattered.Direction(), rec.Normal) > 0);
    }

private:
    Color3 m_Albedo;
    double m_Fuzz;
};

class Dielectric final : public Material
{
public:
    Dielectric(const double indexOfRefraction)
        : m_Ir(indexOfRefraction)
    {
    }

    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const override
    {
        attenuation                  = Color3(1.0, 1.0, 1.0);
        const double refractionRatio = rec.FrontFace ? (1.0 / m_Ir) : m_Ir;

        const Vector3 unitDirection = UnitVector(rIn.Direction());

        const double cosTheta = fmin(DotProduct(-unitDirection, rec.Normal), 1.0);
        const double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

        const bool cannotRefract = refractionRatio * sinTheta > 1.0;
        Vector3    direction;

        if(cannotRefract || Reflectance(cosTheta, refractionRatio) > Random::Double())
        {
            direction = Reflect(unitDirection, rec.Normal);
        }
        else
        {
            direction = Refract(unitDirection, rec.Normal, refractionRatio);
        }

        scattered = Ray(rec.P, direction, rIn.Time());

        return true;
    }

private:
    double m_Ir;    // Index of Refraction

    static double Reflectance(const double cosine, const double refIdx)
    {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refIdx) / (1 + refIdx);
        r0      = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};
