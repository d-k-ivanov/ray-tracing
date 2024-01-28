#pragma once

#include <Render/Color.h>
#include <Render/Hittable.h>
#include <Render/Ray.h>
#include <Render/Texture.h>

#include <memory>

class HitRecord;

class Material
{
public:
    virtual ~Material() = default;
    virtual Color3 Emitted(double u, double v, const Point3& p) const
    {
        return {0, 0, 0};
    }

    virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const = 0;
};

class Lambertian final : public Material
{
public:
    explicit Lambertian(const Color3& color)
        : m_Albedo(std::make_shared<SolidColor>(color))
    {
    }

    explicit Lambertian(const std::shared_ptr<Texture>& texture)
        : m_Albedo(texture)
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
        attenuation = m_Albedo->Value(rec.U, rec.V, rec.P);
        return true;
    }

private:
    std::shared_ptr<Texture> m_Albedo;
};

class Metal final : public Material
{
public:
    explicit Metal(const Color3& color, const double fuzz)
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
    explicit Dielectric(const double indexOfRefraction)
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

class DiffuseLight final : public Material
{
public:
    explicit DiffuseLight(const std::shared_ptr<Texture>& texture)
        : m_Emit(texture)
    {
    }

    explicit DiffuseLight(Color3 color)
        : m_Emit(std::make_shared<SolidColor>(color))
    {
    }

    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const override
    {
        return false;
    }

    Color3 Emitted(const double u, const double v, const Point3& p) const override
    {
        return m_Emit->Value(u, v, p);
    }

private:
    std::shared_ptr<Texture> m_Emit;
};

class Isotropic final : public Material
{
public:
    explicit Isotropic(Color3 color)
        : m_Albedo(std::make_shared<SolidColor>(color))
    {
    }

    explicit Isotropic(const std::shared_ptr<Texture>& texture)
        : m_Albedo(texture)
    {
    }

    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const override
    {
        scattered   = Ray(rec.P, RandomUnitVector(), rIn.Time());
        attenuation = m_Albedo->Value(rec.U, rec.V, rec.P);
        return true;
    }

private:
    std::shared_ptr<Texture> m_Albedo;
};
