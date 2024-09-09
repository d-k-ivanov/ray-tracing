#include "Camera.h"

#include "HitRecord.h"
#include "Material.h"
#include "ScatterRecord.h"

#include <Math/Converters.h>
#include <Math/CosinePDF.h>
#include <Math/HittablePDF.h>
#include <Math/MixturePDF.h>
#include <Objects/Hittable.h>
#include <Objects/HittableList.h>
#include <Utils/Log.h>
#include <Utils/Random.h>

namespace Render
{

// TODO: Original Renderer. Need to take PPM code from here.
// void Camera::Render(const Hittable& world, const hittable& lights)
// {
//     Initialize();
//
//     std::cout << "P3\n";
//     std::cout << ImageWidth << ' ' << m_ImageHeight << "\n";
//     std::cout << "255\n";
//
//     for(int j = 0; j < m_ImageHeight; j++)
//     {
//         std::clog << "\rScanlines remaining: " << (m_ImageHeight - j) << ' ' << std::flush;
//         for(int i = 0; i < ImageWidth; i++)
//         {
//             Color3 pixelColor(0, 0, 0);
//             // for(int sample = 0; sample < SamplesPerPixel; sample++)
//             // {
//             //     Ray r = GetRay(i, j);
//             //     pixelColor += RayColor(r, MaxDepth, world, lights);
//             // }
//              for (int s_j = 0; s_j < SqrtSpp; s_j++) {
//                  for (int s_i = 0; s_i < SqrtSpp; s_i++) {
//                      Ray r = GetRay(i, j, s_i, s_j);
//                      pixelColor += RayColor(r, MaxDepth, world, lights);
//                  }
//              }
//             GetColorRGBA(pixelColor, SamplesPerPixel);
//         }
//     }
//     std::clog << "\rDone.                 \n";
// }

void Camera::Initialize()
{
    m_ImageHeight = static_cast<int>(ImageWidth / AspectRatio);
    m_ImageHeight = (m_ImageHeight < 1) ? 1 : m_ImageHeight;

    m_Center = LookFrom;

    // Determine viewport dimensions.
    const auto theta = Math::DegreesToRadians(Vfov);    // 90 deg == 0.5pi == 1.5707963268 rad
    const auto h     = tan(theta / 2);                  // tan(1.5707963268/2) == 1

    const auto viewportHeight = 2 * h * FocusDist;
    const auto viewportWidth  = viewportHeight * (static_cast<double>(ImageWidth) / m_ImageHeight);

    PixelSamplesScale = 1.0 / SamplesPerPixel;
    SqrtSpp           = static_cast<int>(sqrt(SamplesPerPixel));
    SqrtSppScale      = 1.0 / (SqrtSpp * SqrtSpp);
    m_RecipSqrtSpp    = 1.0 / SqrtSpp;

    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    m_W = Math::UnitVector(LookFrom - LookAt);
    m_U = Math::UnitVector(Math::CrossProduct(VUp, m_W));
    m_V = Math::CrossProduct(m_W, m_U);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    const Math::Vector3 viewportU = viewportWidth * m_U;      // Vector across viewport horizontal edge
    const Math::Vector3 viewportV = viewportHeight * -m_V;    // Vector down viewport vertical edge

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    m_PixelDeltaU = viewportU / ImageWidth;
    m_PixelDeltaV = viewportV / m_ImageHeight;

    // Calculate the location of the upper left pixel.
    const auto viewportUpperLeft = m_Center - (FocusDist * m_W) - viewportU / 2 - viewportV / 2;
    m_Pixel00Loc                 = viewportUpperLeft + 0.5 * (m_PixelDeltaU + m_PixelDeltaV);

    // Calculate the camera defocus disk basis vectors.
    const auto defocusRadius = FocusDist * tan(Math::DegreesToRadians(DefocusAngle / 2));
    m_DefocusDiskU           = m_U * defocusRadius;
    m_DefocusDiskV           = m_V * defocusRadius;
    // LOG_INFO("Camera. PDF={}, ULight={}, SqrtSpp={}, RecipSqrtSpp={}, Samples={}", UsePDF, UseUnidirectionalLight,SqrtSpp, m_RecipSqrtSpp, SamplesPerPixel);
}

uint32_t Camera::GetPixel(const uint32_t x, const uint32_t y, const Objects::Hittable& world, const Objects::HittableList& lights) const
{
    Color3 pixelColor(0, 0, 0);
    if(SamplingType == SamplerType::Stratified)
    {
        for(int yS = 0; yS < this->SqrtSpp; yS++)
        {
            for(int xS = 0; xS < this->SqrtSpp; xS++)
            {
                Ray r = this->GetRay(static_cast<int>(x), static_cast<int>(y), xS, yS);
                pixelColor += this->RayColor(r, this->MaxDepth, world, lights);
            }
        }
    }
    else
    {
        for(int sample = 0; sample < this->SamplesPerPixel; sample++)
        {
            Ray r = this->GetRay(static_cast<int>(x), static_cast<int>(y));
            pixelColor += this->RayColor(r, this->MaxDepth, world, lights);
        }
    }
    return GetColorRGBA(pixelColor, this->PixelSamplesScale);
}

// Construct a camera ray originating from the defocus disk and directed at a randomly
// sampled point around the pixel location x, y for stratified sample square xS, yS
Ray Camera::GetRay(const int x, const int y, const int xS, const int yS) const
{
    Math::Vector3 pixelSampleOffset;

    // if required to sample pixels randomly around the pixel location (stratified sampling)
    if(SamplingType == SamplerType::Stratified)
    {
        pixelSampleOffset = SampleSquare(xS, yS);
        // pixelSampleOffset = SampleDisk(xS, yS);
    }
    else
    {
        pixelSampleOffset = SampleSquare();
        // pixelSampleOffset = SampleDisk();
    }

    // const auto pixelSample = m_Pixel00Lkoc + (x * m_PixelDeltaU) + (y * m_PixelDeltaV);

    const auto pixelSample = m_Pixel00Loc
                             + ((x + pixelSampleOffset.X()) * m_PixelDeltaU)
                             + ((y + pixelSampleOffset.Y()) * m_PixelDeltaV);

    auto       rayOrigin    = (DefocusAngle <= 0) ? m_Center : DefocusDiskSample();
    const auto rayDirection = pixelSample - rayOrigin;
    const auto rayTime      = Utils::Random::Double();

    return {rayOrigin, rayDirection, rayTime};
}

Color3 Camera::RayColorGradientBackground(const Ray& r, const int depth, const Objects::Hittable& world) const
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if(depth <= 0)
    {
        return {0, 0, 0};
    }

    HitRecord rec;

    if(world.Hit(r, Math::Interval(0.001, Math::Infinity), rec))
    {
        Ray    scattered;
        Color3 attenuation;
        if(rec.GetMaterial()->Scatter(r, rec, attenuation, scattered))
        {
            return attenuation * RayColorGradientBackground(scattered, depth - 1, world);
        }
        return {0, 0, 0};
    }

    const Math::Vector3 unitDirection = Math::UnitVector(r.Direction());

    const auto a = 0.5 * (unitDirection.Y() + 1.0);
    return (1.0 - a) * Color3(1.0, 1.0, 1.0) + a * Color3(0.5, 0.7, 1.0);
    // return (1.0 - a) * Color3(1.0, 1.0, 1.0) + a * Color3(1.0, 0.0, 0.0);
}

Color3 Camera::RayColor(const Ray& r, const int depth, const Objects::Hittable& world, const Objects::HittableList& lights) const
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if(depth <= 0)
    {
        return {0, 0, 0};
    }

    HitRecord rec;

    // If the ray hits nothing, return the background color.
    if(!world.Hit(r, Math::Interval(0.001, Math::Infinity), rec))
    {
        return Background;
    }

    Color3 colorFromEmission;
    Color3 colorFromScatter;

    if(UseUnidirectionalLight)
    {
        colorFromEmission = rec.GetMaterial()->Emitted(r, rec, rec.U, rec.V, rec.P);
    }
    else
    {
        colorFromEmission = rec.GetMaterial()->Emitted(rec.U, rec.V, rec.P);
    }

    if(UsePDF)
    {
        ScatterRecord srec;

        if(!rec.GetMaterial()->Scatter(r, rec, srec))
        {
            return colorFromEmission;
        }

        if(srec.SkipPDF)
        {
            return srec.Attenuation * RayColor(srec.SkipPDFRay, depth - 1, world, lights);
        }

        std::shared_ptr<Math::PDF> pdfPtr;
        if(lights.Objects.empty())
        {
            // const CosinePDF surfacePDF(rec.Normal);
            // const Ray       scattered = Ray(rec.P, surfacePDF.Generate(), r.Time());
            // const auto      pdfVal    = surfacePDF.Value(scattered.Direction());
            pdfPtr = std::make_shared<Math::CosinePDF>(rec.Normal);
        }
        else
        {
            pdfPtr = std::make_shared<Math::HittablePDF>(lights, rec.P);
        }

        const Math::MixturePDF mixedPDF(pdfPtr, srec.PDFPtr);

        const Ray  scattered = Ray(rec.P, mixedPDF.Generate(), r.Time());
        const auto pdfVal    = mixedPDF.Value(scattered.Direction());

        // Scattering is impossible
        if(std::isnan(pdfVal) || Math::DoubleUtils::isEqual(pdfVal, 0.0, Math::DoubleUtils::DefaultTolerance()))
        {
            return colorFromEmission;
        }

        const double scatteringPDF = rec.GetMaterial()->ScatteringPDF(r, rec, scattered);

        const Color3 sampleColor = RayColor(scattered, depth - 1, world, lights);
        colorFromScatter         = (srec.Attenuation * scatteringPDF * sampleColor) / pdfVal;
    }
    else
    {
        Ray    scattered;
        Color3 attenuation;

        if(!rec.GetMaterial()->Scatter(r, rec, attenuation, scattered))
        {
            return colorFromEmission;
        }

        colorFromScatter = attenuation * RayColor(scattered, depth - 1, world, lights);
    }

    return colorFromEmission + colorFromScatter;
}

// Returns a random point in the camera defocus disk.
Math::Point3 Camera::DefocusDiskSample() const
{
    auto p = Math::RandomInUnitDisk();
    return m_Center + (p[0] * m_DefocusDiskU) + (p[1] * m_DefocusDiskV);
}

// Returns the vector to a random point in the [-0.5,-0.5]-[+0.5,+0.5] unit square.
Math::Vector3 Camera::SampleSquare() const
{
    return {Utils::Random::Double() - 0.5, Utils::Random::Double() - 0.5, 0};
}

// Returns the vector to a random point in the square sub-pixel specified by grid
// indices xS and yS, for an idealized unit square pixel [-0.5,-0.5] to [+0.5,+0.5]
Math::Vector3 Camera::SampleSquare(const int xS, const int yS) const
{
    const auto px = ((xS + Utils::Random::Double()) * m_RecipSqrtSpp) - 0.5;
    const auto py = ((yS + Utils::Random::Double()) * m_RecipSqrtSpp) - 0.5;
    return {px, py, 0};
}

// Returns the vector to a random point in the [-0.5,-0.5]-[+0.5,+0.5] unit circle.
Math::Vector3 Camera::SampleDisk() const
{
    return 0.5 * Math::RandomInUnitDisk();
}

// Returns the vector to a random point in the circle sub-pixel specified by grid
// indices xS and yS, for an idealized unit circle pixel [-0.5,-0.5] to [+0.5,+0.5]
Math::Vector3 Camera::SampleDisk(const int xS, const int yS) const
{
    const auto px = ((xS + Math::RandomInUnitDisk().X()) * m_RecipSqrtSpp) - 0.5;
    const auto py = ((yS + Math::RandomInUnitDisk().Y()) * m_RecipSqrtSpp) - 0.5;
    return {px, py, 0};
}

}    // namespace Render
