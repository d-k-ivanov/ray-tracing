#pragma once

#include "Color.h"
#include "Ray.h"
#include "Texture.h"

#include <Math/PDF.h>
#include <Objects/Hittable.h>

#include <memory>

class HitRecord;

class ScatterRecord
{
public:
    Color3               Attenuation;
    std::shared_ptr<PDF> PDFPtr;
    bool                 SkipPDF;
    Ray                  SkipPDFRay;
};

class Material
{
public:
    virtual ~Material() = default;

    virtual Color3 Emitted(double u, double v, const Point3& p) const;
    virtual Color3 Emitted(const Ray& rIn, const HitRecord& rec, double u, double v, const Point3& p) const;

    virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const              = 0;
    virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const = 0;
    virtual bool Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const;

    virtual double ScatteringPDF(const Ray& rIn, const HitRecord& rec, const Ray& scattered) const;
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

    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const override;

    double ScatteringPDF(const Ray& rIn, const HitRecord& rec, const Ray& scattered) const override;

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

    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const override;

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

    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const override;

private:
    double m_Ir;    // Index of Refraction

    static double Reflectance(double cosine, double refIdx);
};

class DiffuseLight final : public Material
{
public:
    explicit DiffuseLight(const std::shared_ptr<Texture>& texture)
        : m_Emit(texture)
    {
    }

    explicit DiffuseLight(const Color3& color)
        : m_Emit(std::make_shared<SolidColor>(color))
    {
    }

    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const override;

    Color3 Emitted(double u, double v, const Point3& p) const override;
    Color3 Emitted(const Ray& rIn, const HitRecord& rec, double u, double v, const Point3& p) const override;

private:
    std::shared_ptr<Texture> m_Emit;
};

class Isotropic final : public Material
{
public:
    explicit Isotropic(const Color3& color)
        : m_Albedo(std::make_shared<SolidColor>(color))
    {
    }

    explicit Isotropic(const std::shared_ptr<Texture>& texture)
        : m_Albedo(texture)
    {
    }

    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, Color3& attenuation, Ray& scattered, double& pdf) const override;
    bool Scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const override;

    double ScatteringPDF(const Ray& rIn, const HitRecord& rec, const Ray& scattered) const override;

private:
    std::shared_ptr<Texture> m_Albedo;
};
