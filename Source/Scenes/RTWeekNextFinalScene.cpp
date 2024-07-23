#include "Scene.h"

#include <Math/Vector3.h>
#include <Objects/BVH.h>
#include <Objects/Box.h>
#include <Objects/ConstantMedium.h>
#include <Objects/Quad.h>
#include <Objects/RotateY.h>
#include <Objects/Sphere.h>
#include <Objects/Translate.h>
#include <Render/Dielectric.h>
#include <Render/DiffuseLight.h>
#include <Render/ImageTexture.h>
#include <Render/Lambertian.h>
#include <Render/Metal.h>
#include <Render/NoiseTexture.h>
#include <Utils/Filesystem.h>

#include <memory>

namespace Scenes
{

RTWeekNextFinalScene::RTWeekNextFinalScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    Objects::HittableList boxes1;
    const auto            ground = std::make_shared<Render::Lambertian>(Render::Color3(0.48, 0.83, 0.53));

    constexpr int boxesPerSide = 20;
    for(int i = 0; i < boxesPerSide; i++)
    {
        for(int j = 0; j < boxesPerSide; j++)
        {
            constexpr auto w  = 100.0;
            const auto     x0 = -1000.0 + i * w;
            const auto     z0 = -1000.0 + j * w;
            constexpr auto y0 = 0.0;
            const auto     x1 = x0 + w;
            const auto     y1 = Utils::Random::Double(1, 101);
            const auto     z1 = z0 + w;

            boxes1.Add(Objects::Box(Math::Point3(x0, y0, z0), Math::Point3(x1, y1, z1), ground));
        }
    }

    m_World.Add(std::make_shared<Objects::BVHNode>(boxes1));

    // Lights
    auto light = std::make_shared<Render::DiffuseLight>(Render::Color3(7, 7, 7));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(123, 554, 147), Math::Vector3(300, 0, 0), Math::Vector3(0, 0, 265), light));
    m_Lights.Add(std::make_shared<Objects::Quad>(Math::Point3(123, 554, 147), Math::Vector3(299, 0, 0), Math::Vector3(0, 0, 264), light));

    auto center1        = Math::Point3(400, 400, 200);
    auto center2        = center1 + Math::Vector3(30, 0, 0);
    auto sphereMaterial = std::make_shared<Render::Lambertian>(Render::Color3(0.7, 0.3, 0.1));
    m_World.Add(std::make_shared<Objects::Sphere>(center1, center2, 50, sphereMaterial));

    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(260, 150, 45), 50, std::make_shared<Render::Dielectric>(1.5)));
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(0, 150, 145), 50, std::make_shared<Render::Metal>(Render::Color3(0.8, 0.8, 0.9), 1.0)));

    auto boundary = std::make_shared<Objects::Sphere>(Math::Point3(360, 150, 145), 70, std::make_shared<Render::Dielectric>(1.5));
    m_World.Add(boundary);
    m_World.Add(std::make_shared<Objects::ConstantMedium>(boundary, 0.2, Render::Color3(0.2, 0.4, 0.9)));
    boundary = std::make_shared<Objects::Sphere>(Math::Point3(0, 0, 0), 5000, std::make_shared<Render::Dielectric>(1.5));
    m_World.Add(std::make_shared<Objects::ConstantMedium>(boundary, .0001, Render::Color3(1, 1, 1)));

    const auto earthTexturePath = Utils::ThisExecutableLocation() + "/Resources/Textures/earthmap.jpg";
    auto       earthMaterial    = std::make_shared<Render::Lambertian>(std::make_shared<Render::ImageTexture>(earthTexturePath));
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(400, 200, 400), 100, earthMaterial));

    auto pertext = std::make_shared<Render::NoiseTexture>(0.1);
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(220, 280, 300), 80, std::make_shared<Render::Lambertian>(pertext)));

    Objects::HittableList boxes2;
    auto                  white = std::make_shared<Render::Lambertian>(Render::Color3(.73, .73, .73));
    constexpr int         ns    = 1000;
    for(int j = 0; j < ns; j++)
    {
        boxes2.Add(std::make_shared<Objects::Sphere>(Math::Point3::Random(0, 165), 10, white));
    }

    m_World.Add(std::make_shared<Objects::Translate>(std::make_shared<Objects::RotateY>(std::make_shared<Objects::BVHNode>(boxes2), 15), Math::Vector3(-100, 270, 395)));

    m_Camera.AspectRatio     = m_AspectRatio;
    m_Camera.ImageWidth      = m_Width;
    m_Camera.SamplesPerPixel = m_SamplesPerPixel;
    m_Camera.MaxDepth        = m_MaxDepth;
    m_Camera.Background      = Render::Color3(0, 0, 0);

    m_Camera.Vfov     = 40;
    m_Camera.LookFrom = Math::Point3(478, 278, -600);
    m_Camera.LookAt   = Math::Point3(278, 278, 0);
    m_Camera.VUp      = Math::Vector3(0, 1, 0);

    m_Camera.DefocusAngle = 0;
}

}    // namespace Scenes
