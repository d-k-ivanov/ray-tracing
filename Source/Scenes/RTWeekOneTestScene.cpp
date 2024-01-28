#include "Scene.h"

#include <Math/Vector3.h>
#include <Render/Material.h>
#include <Render/Sphere.h>

RTWeekOneTestScene::RTWeekOneTestScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
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
