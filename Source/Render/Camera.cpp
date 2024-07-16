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
    const auto theta = DegreesToRadians(Vfov);    // 90 deg == 0.5pi == 1.5707963268 rad
    const auto h     = tan(theta / 2);            // tan(1.5707963268/2) == 1

    const auto viewportHeight = 2 * h * FocusDist;
    const auto viewportWidth  = viewportHeight * (static_cast<double>(ImageWidth) / m_ImageHeight);

    PixelSampleScale = 1.0 / SamplesPerPixel;
    SqrtSpp          = static_cast<int>(sqrt(SamplesPerPixel));
    SqrtSppScale     = 1.0 / (SqrtSpp * SqrtSpp);
    m_RecipSqrtSpp   = 1.0 / SqrtSpp;

    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    m_W = UnitVector(LookFrom - LookAt);
    m_U = UnitVector(CrossProduct(VUp, m_W));
    m_V = CrossProduct(m_W, m_U);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    const Vector3 viewportU = viewportWidth * m_U;      // Vector across viewport horizontal edge
    const Vector3 viewportV = viewportHeight * -m_V;    // Vector down viewport vertical edge

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    m_PixelDeltaU = viewportU / ImageWidth;
    m_PixelDeltaV = viewportV / m_ImageHeight;

    // Calculate the location of the upper left pixel.
    const auto viewportUpperLeft = m_Center - (FocusDist * m_W) - viewportU / 2 - viewportV / 2;
    m_Pixel00Loc                 = viewportUpperLeft + 0.5 * (m_PixelDeltaU + m_PixelDeltaV);

    // Calculate the camera defocus disk basis vectors.
    const auto defocusRadius = FocusDist * tan(DegreesToRadians(DefocusAngle / 2));
    m_DefocusDiskU           = m_U * defocusRadius;
    m_DefocusDiskV           = m_V * defocusRadius;
    // LOG_INFO("Camera. PDF={}, ULight={}, SqrtSpp={}, RecipSqrtSpp={}, Samples={}", UsePDF, UseUnidirectionalLight,SqrtSpp, m_RecipSqrtSpp, SamplesPerPixel);
}

// Get a randomly-sampled camera ray for the pixel at location i,j, originating from the camera defocus disk.
Ray Camera::GetRay(const int i, const int j) const
{
    const auto pixelCenter = m_Pixel00Loc + (i * m_PixelDeltaU) + (j * m_PixelDeltaV);
    const auto pixelSample = pixelCenter + PixelSampleSquare();
    // const auto pixelSample = pixelCenter + RandomInUnitDisk();

    auto       rayOrigin    = (DefocusAngle <= 0) ? m_Center : DefocusDiskSample();
    const auto rayDirection = pixelSample - rayOrigin;
    const auto rayTime      = Random::Double();

    return {rayOrigin, rayDirection, rayTime};
}

// Get a randomly-sampled camera ray for the pixel at location i,j,
// originating from the camera defocus disk, and randomly sampled around the pixel location.
Ray Camera::GetRay(const int i, const int j, const int iS, const int jS) const
{
    const auto pixelCenter = m_Pixel00Loc + (i * m_PixelDeltaU) + (j * m_PixelDeltaV);
    const auto pixelSample = pixelCenter + PixelSampleSquare(iS, jS);
    // const auto pixelSample = pixelCenter + RandomInUnitDisk();

    auto       rayOrigin    = (DefocusAngle <= 0) ? m_Center : DefocusDiskSample();
    const auto rayDirection = pixelSample - rayOrigin;
    const auto rayTime      = Random::Double();

    return {rayOrigin, rayDirection, rayTime};
}

Color3 Camera::RayColorGradientBackground(const Ray& r, const int depth, const Hittable& world) const
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if(depth <= 0)
    {
        return {0, 0, 0};
    }

    HitRecord rec;

    if(world.Hit(r, Interval(0.001, Infinity), rec))
    {
        Ray    scattered;
        Color3 attenuation;
        if(rec.GetMaterial()->Scatter(r, rec, attenuation, scattered))
        {
            return attenuation * RayColor(scattered, depth - 1, world);
        }
        return {0, 0, 0};
    }

    const Vector3 unitDirection = UnitVector(r.Direction());
    const auto    a             = 0.5 * (unitDirection.Y() + 1.0);
    return (1.0 - a) * Color3(1.0, 1.0, 1.0) + a * Color3(0.5, 0.7, 1.0);
    // return (1.0 - a) * Color3(1.0, 1.0, 1.0) + a * Color3(1.0, 0.0, 0.0);
}

Color3 Camera::RayColor(const Ray& r, const int depth, const Hittable& world) const
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if(depth <= 0)
    {
        return {0, 0, 0};
    }

    HitRecord rec;

    // If the ray hits nothing, return the background color.
    if(!world.Hit(r, Interval(0.001, Infinity), rec))
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
            return srec.Attenuation * RayColor(srec.SkipPDFRay, depth - 1, world);
        }

        // const CosinePDF surfacePDF(rec.Normal);
        // const Ray       scattered = Ray(rec.P, surfacePDF.Generate(), r.Time());
        // const auto      pdfVal    = surfacePDF.Value(scattered.Direction());

        const auto       pdfPtr = std::make_shared<CosinePDF>(rec.Normal);
        const MixturePDF mixedPDF(pdfPtr, srec.PDFPtr);

        const Ray  scattered = Ray(rec.P, mixedPDF.Generate(), r.Time());
        const auto pdfVal    = mixedPDF.Value(scattered.Direction());

        // Scattering is impossible
        if(isnan(pdfVal) || DoubleUtils::isEqual(pdfVal, 0.0, DoubleUtils::DefaultTolerance()))
        {
            return colorFromEmission;
        }

        const double scatteringPDF = rec.GetMaterial()->ScatteringPDF(r, rec, scattered);

        const Color3 sampleColor = RayColor(scattered, depth - 1, world);
        colorFromScatter         = (srec.Attenuation * scatteringPDF * sampleColor) / pdfVal;
    }
    else
    {
        Ray    scattered;
        Color3 attenuation;

        if(!rec.GetMaterial()->Scatter(r, rec, attenuation, scattered))
            return colorFromEmission;

        colorFromScatter = attenuation * RayColor(scattered, depth - 1, world);
    }

    return colorFromEmission + colorFromScatter;
}

Color3 Camera::RayColor(const Ray& r, const int depth, const Hittable& world, const Hittable& lights) const
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if(depth <= 0)
    {
        return {0, 0, 0};
    }

    HitRecord rec;

    // If the ray hits nothing, return the background color.
    if(!world.Hit(r, Interval(0.001, Infinity), rec))
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

        const auto       lightPtr = std::make_shared<HittablePDF>(lights, rec.P);
        const MixturePDF mixedPDF(lightPtr, srec.PDFPtr);

        const Ray  scattered = Ray(rec.P, mixedPDF.Generate(), r.Time());
        const auto pdfVal    = mixedPDF.Value(scattered.Direction());

        // Scattering is impossible
        if(isnan(pdfVal) || DoubleUtils::isEqual(pdfVal, 0.0, DoubleUtils::DefaultTolerance()))
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
            return colorFromEmission;

        colorFromScatter = attenuation * RayColor(scattered, depth - 1, world);
    }

    return colorFromEmission + colorFromScatter;
}

uint32_t Camera::GetPixel(const uint32_t x, const uint32_t y, const Hittable& world, const HittableList& lights) const
{
    Color3 pixelColor(0, 0, 0);
    for(int sample = 0; sample < this->SamplesPerPixel; sample++)
    {
        Ray r = this->GetRay(static_cast<int>(x), static_cast<int>(y));
        if(lights.Objects.empty())
        {
            pixelColor += this->RayColor(r, this->MaxDepth, world);
        }
        else
        {
            pixelColor += this->RayColor(r, this->MaxDepth, world, lights);
        }
    }
    return GetColorRGBA(pixelColor, this->PixelSampleScale);
}

// Returns a random point in the square surrounding a pixel at the origin.
Vector3 Camera::PixelSampleSquare() const
{
    const auto px = -0.5 + Random::Double();
    const auto py = -0.5 + Random::Double();
    return (px * m_PixelDeltaU) + (py * m_PixelDeltaV);
}

// Returns a random point in the square surrounding a pixel at the origin, given the two subpixel indices.
Vector3 Camera::PixelSampleSquare(const int iS, const int jS) const
{
    const auto px = -0.5 + m_RecipSqrtSpp * (iS + Random::Double());
    const auto py = -0.5 + m_RecipSqrtSpp * (jS + Random::Double());
    return (px * m_PixelDeltaU) + (py * m_PixelDeltaV);
}

// Generate a sample from the disk of given radius around a pixel at the origin.
Vector3 Camera::PixelSampleDisk(const double radius) const
{
    auto p = radius * RandomInUnitDisk();
    return (p[0] * m_PixelDeltaU) + (p[1] * m_PixelDeltaV);
}

// Returns a random point in the camera defocus disk.
Point3 Camera::DefocusDiskSample() const
{
    auto p = RandomInUnitDisk();
    return m_Center + (p[0] * m_DefocusDiskU) + (p[1] * m_DefocusDiskV);
}
