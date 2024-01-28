// ReSharper disable CppClangTidyModernizeUseNodiscard
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppTooWideScopeInitStatement
#pragma once

#include <Math/Converters.h>
#include <Render/Color.h>
#include <Render/Hittable.h>
#include <Render/Material.h>
#include <Utils/Random.h>

class Camera
{
public:
    double AspectRatio     = 1.0;    // Ratio of image width over height
    int    ImageWidth      = 100;    // Rendered image width in pixel count
    int    SamplesPerPixel = 10;     // Count of random samples for each pixel
    int    MaxDepth        = 10;     // Maximum number of ray bounces into scene
    Color3 Background;               // Scene background color

    double  Vfov     = 90;                  // Vertical view angle (field of view)
    Point3  LookFrom = Point3(0, 0, 0);     // Point camera is looking from
    Point3  LookAt   = Point3(0, 0, -1);    // Point camera is looking at
    Vector3 VUp      = Vector3(0, 1, 0);    // Camera-relative "up" direction

    double DefocusAngle = 0;     // Variation angle of rays through each pixel
    double FocusDist    = 10;    // Distance from camera lookfrom point to plane of perfect focus

    // TODO: Original Renderer, Need to move my custom rendering functions here.
    void Render(const Hittable& world)
    {
        Initialize();

        std::cout << "P3\n";
        std::cout << ImageWidth << ' ' << m_ImageHeight << "\n";
        std::cout << "255\n";

        for(int j = 0; j < m_ImageHeight; ++j)
        {
            std::clog << "\rScanlines remaining: " << (m_ImageHeight - j) << ' ' << std::flush;
            for(int i = 0; i < ImageWidth; ++i)
            {
                Color3 pixelColor(0, 0, 0);
                for(int sample = 0; sample < SamplesPerPixel; ++sample)
                {
                    Ray r = GetRay(i, j);
                    pixelColor += RayColor(r, MaxDepth, world);
                }
                GetColorRGBA(pixelColor, SamplesPerPixel);
            }
        }
        std::clog << "\rDone.                 \n";
    }

    void Initialize()
    {
        m_ImageHeight = static_cast<int>(ImageWidth / AspectRatio);
        m_ImageHeight = (m_ImageHeight < 1) ? 1 : m_ImageHeight;

        m_Center = LookFrom;

        // Determine viewport dimensions.
        const auto theta = DegreesToRadians(Vfov);    // 90 deg == 0.5pi == 1.5707963268 rad
        const auto h     = tan(theta / 2);            // tan(1.5707963268/2) == 1

        const auto viewportHeight = 2 * h * FocusDist;
        const auto viewportWidth  = viewportHeight * (static_cast<double>(ImageWidth) / m_ImageHeight);

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
    }

    // Get a randomly-sampled camera ray for the pixel at location i,j, originating from the camera defocus disk.
    Ray GetRay(const int i, const int j) const
    {
        const auto pixelCenter = m_Pixel00Loc + (i * m_PixelDeltaU) + (j * m_PixelDeltaV);
        const auto pixelSample = pixelCenter + PixelSampleSquare();
        // const auto pixelSample = pixelCenter + RandomInUnitDisk();

        auto       rayOrigin    = (DefocusAngle <= 0) ? m_Center : DefocusDiskSample();
        const auto rayDirection = pixelSample - rayOrigin;
        const auto rayTime      = Random::Double();

        return {rayOrigin, rayDirection, rayTime};
    }

    Color3 RayColor(const Ray& r, const int depth, const Hittable& world) const
    {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if(depth <= 0)
        {
            return {0, 0, 0};
        }

        HitRecord rec;

        // TBA: If the ray hits nothing, return the background color.
        // if (!world.Hit(r, Interval(0.001, Infinity), rec))
        //     return Background;


        if(world.Hit(r, Interval(0.001, Infinity), rec))
        {
            Ray    scattered;
            Color3 attenuation;
            if(rec.Material->Scatter(r, rec, attenuation, scattered))
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

private:
    int     m_ImageHeight = 0;    // Rendered image height
    Point3  m_Center;             // Camera m_center
    Point3  m_Pixel00Loc;         // Location of pixel 0, 0
    Vector3 m_PixelDeltaU;        // Offset to pixel to the right
    Vector3 m_PixelDeltaV;        // Offset to pixel below
    Vector3 m_U;                  // Camera frame basis vectors
    Vector3 m_V;                  // Camera frame basis vectors
    Vector3 m_W;                  // Camera frame basis vectors
    Vector3 m_DefocusDiskU;       // Defocus disk horizontal radius
    Vector3 m_DefocusDiskV;       // Defocus disk vertical radius

    // Returns a random point in the square surrounding a pixel at the origin.
    Vector3 PixelSampleSquare() const
    {
        const auto px = -0.5 + Random::Double();
        const auto py = -0.5 + Random::Double();
        return (px * m_PixelDeltaU) + (py * m_PixelDeltaV);
    }

    // Generate a sample from the disk of given radius around a pixel at the origin.
    Vector3 PixelSampleDisk(const double radius) const
    {
        auto p = radius * RandomInUnitDisk();
        return (p[0] * m_PixelDeltaU) + (p[1] * m_PixelDeltaV);
    }

    // Returns a random point in the camera defocus disk.
    Point3 DefocusDiskSample() const
    {
        auto p = RandomInUnitDisk();
        return m_Center + (p[0] * m_DefocusDiskU) + (p[1] * m_DefocusDiskV);
    }
};
