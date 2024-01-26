#include "Scene.h"

#include <Math/Vector3.h>
#include <Render/Material.h>
#include <Render/Sphere.h>
#include <Utils/Random.h>

RtOneSceneWIP::RtOneSceneWIP(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : m_AspectRatio(aspectRatio)
    , m_Width(width)
    , m_SamplesPerPixel(samplesPerPixel)
    , m_MaxDepth(maxDepth)
{
    // Metals
    // auto materialGround = std::make_shared<Lambertian>(Color3(0.8, 0.8, 0.0));
    // auto materialCenter = std::make_shared<Lambertian>(Color3(0.7, 0.3, 0.3));
    // auto materialLeft   = std::make_shared<Metal>(Color3(0.8, 0.8, 0.8), 0.3);
    // auto materialRight  = std::make_shared<Metal>(Color3(0.8, 0.6, 0.2), 1.0);

    // Dielectric 1:
    // auto materialGround = std::make_shared<Lambertian>(Color3(0.8, 0.8, 0.0));
    // auto material_center = std::make_shared<Dielectric>(1.5);
    // auto materialLeft   = std::make_shared<Dielectric>(1.5);
    // auto materialRight  = std::make_shared<Metal>(Color3(0.8, 0.6, 0.2), 1.0);

    // Dielectric 2:
    // auto materialGround = std::make_shared<Lambertian>(Color3(0.8, 0.8, 0.0));
    // auto materialCenter = std::make_shared<Lambertian>(Color3(0.1, 0.2, 0.5));
    // auto materialLeft   = std::make_shared<Dielectric>(1.5);
    // auto materialRight  = std::make_shared<Metal>(Color3(0.8, 0.6, 0.2), 0.0);

    // m_World.Add(std::make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, materialGround));
    // m_World.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1.0), 0.5, materialCenter));
    // m_World.Add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, materialLeft));
    // m_World.Add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), -0.4, materialLeft));
    // m_World.Add(std::make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, materialRight));

    // Camera Test
    auto r             = cos(Pi / 4);
    auto materialLeft  = std::make_shared<Lambertian>(Color3(0, 0, 1));
    auto materialRight = std::make_shared<Lambertian>(Color3(1, 0, 0));
    //
    m_World.Add(std::make_shared<Sphere>(Point3(-r, 0, -1), r, materialLeft));
    m_World.Add(std::make_shared<Sphere>(Point3(r, 0, -1), r, materialRight));

    m_Camera.AspectRatio     = m_AspectRatio;
    m_Camera.ImageWidth      = m_Width;
    m_Camera.SamplesPerPixel = m_SamplesPerPixel;
    m_Camera.MaxDepth        = m_MaxDepth;

    // m_Camera.Vfov     = 90;
    // m_Camera.Vfov     = 20;
    // m_Camera.LookFrom = Point3(-2, 2, 1);
    // m_Camera.LookFrom = Point3(0, 0, 0);
    // m_Camera.LookAt   = Point3(0, 0, -1);
    // m_Camera.VUp      = Vector3(0, 1, 0);


    m_Camera.DefocusAngle = 1.0;
    m_Camera.FocusDist    = 1.0;
    // m_Camera.DefocusAngle = 10.0;
    // m_Camera.FocusDist    = 3.4;
}

RtOneSceneFinal::RtOneSceneFinal(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : m_AspectRatio(aspectRatio)
    , m_Width(width)
    , m_SamplesPerPixel(samplesPerPixel)
    , m_MaxDepth(maxDepth)
{
    auto groundMaterial = std::make_shared<Lambertian>(Color3(0.5, 0.5, 0.5));
    m_World.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, groundMaterial));

    for(int a = -11; a < 11; a++)
    {
        for(int b = -11; b < 11; b++)
        {
            const auto chooseMaterial = Random::Double();
            Point3     center(a + 0.9 * Random::Double(), 0.2, b + 0.9 * Random::Double());

            if((center - Point3(4, 0.2, 0)).Length() > 0.9)
            {
                std::shared_ptr<Material> sphereMaterial;

                if(chooseMaterial < 0.8)
                {
                    // diffuse
                    auto albedo    = Color3::Random() * Color3::Random();
                    sphereMaterial = std::make_shared<Lambertian>(albedo);
                    m_World.Add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
                else if(chooseMaterial < 0.95)
                {
                    // metal
                    auto albedo    = Color3::Random(0.5, 1);
                    auto fuzz      = Random::Double(0, 0.5);
                    sphereMaterial = std::make_shared<Metal>(albedo, fuzz);
                    m_World.Add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
                else
                {
                    // glass
                    sphereMaterial = std::make_shared<Dielectric>(1.5);
                    m_World.Add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    m_World.Add(std::make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Color3(0.5, 0.4, 0.7));
    m_World.Add(std::make_shared<Sphere>(Point3(0, 1, 3), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Color3(0.7, 0.6, 0.5), 0.0);
    m_World.Add(std::make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    m_Camera.AspectRatio     = m_AspectRatio;
    m_Camera.ImageWidth      = m_Width;
    m_Camera.SamplesPerPixel = m_SamplesPerPixel;
    m_Camera.MaxDepth        = m_MaxDepth;

    m_Camera.Vfov     = 20;
    m_Camera.LookFrom = Point3(13, 2, 3);
    m_Camera.LookAt   = Point3(0, 0, 0);
    m_Camera.VUp      = Vector3(0, 1, 0);

    m_Camera.DefocusAngle = 0.6;
    m_Camera.FocusDist    = 10.0;
};
