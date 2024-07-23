#include "Scene.h"

#include <Math/Vector3.h>
#include <Objects/BVH.h>
#include <Objects/Sphere.h>
#include <Render/CheckerTexture.h>
#include <Render/Dielectric.h>
#include <Render/Lambertian.h>
#include <Render/Metal.h>
#include <Utils/Random.h>

#include <memory>

namespace Scenes
{

RTWeekNextRandomSpheresScene::RTWeekNextRandomSpheresScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    // auto groundMaterial = std::make_shared<Lambertian>(Color3(0.5, 0.5, 0.5));
    // m_World.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, groundMaterial));

    auto checker = std::make_shared<Render::CheckerTexture>(0.32, Render::Color3(.2, .3, .1), Render::Color3(.9, .9, .9));
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(0, -1000, 0), 1000, std::make_shared<Render::Lambertian>(checker)));

    auto lightMaterial = std::shared_ptr<Render::Material>();
    m_Lights.Add(std::make_shared<Objects::Sphere>(Math::Point3(0, 15, 0), 2, lightMaterial));

    for(int a = -11; a < 11; a++)
    {
        for(int b = -11; b < 11; b++)
        {
            const auto   chooseMaterial = Utils::Random::Double();
            Math::Point3 center(a + 0.9 * Utils::Random::Double(), 0.2, b + 0.9 * Utils::Random::Double());

            if((center - Math::Point3(4, 0.2, 0)).Length() > 0.9)
            {
                std::shared_ptr<Render::Material> sphereMaterial;

                if(chooseMaterial < 0.8)
                {
                    // diffuse
                    auto albedo    = Render::Color3::Random() * Render::Color3::Random();
                    sphereMaterial = std::make_shared<Render::Lambertian>(albedo);
                    auto center2   = center + Math::Vector3(0, Utils::Random::Double(0.0, 0.5), 0);
                    m_World.Add(std::make_shared<Objects::Sphere>(center, center2, 0.2, sphereMaterial));
                }
                else if(chooseMaterial < 0.95)
                {
                    // metal
                    auto albedo    = Render::Color3::Random(0.5, 1);
                    auto fuzz      = Utils::Random::Double(0, 0.5);
                    sphereMaterial = std::make_shared<Render::Metal>(albedo, fuzz);
                    m_World.Add(std::make_shared<Objects::Sphere>(center, 0.2, sphereMaterial));
                }
                else
                {
                    // glass
                    sphereMaterial = std::make_shared<Render::Dielectric>(1.5);
                    m_World.Add(std::make_shared<Objects::Sphere>(center, 0.2, sphereMaterial));
                }
            }
        }
    }

    auto material1 = std::make_shared<Render::Dielectric>(1.5);
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Render::Lambertian>(Render::Color3(0.5, 0.4, 0.7));
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(0, 1, 3), 1.0, material2));

    auto material3 = std::make_shared<Render::Metal>(Render::Color3(0.7, 0.6, 0.5), 0.0);
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(4, 1, 0), 1.0, material3));

    m_World = Objects::HittableList(std::make_shared<Objects::BVHNode>(m_World));

    m_Camera.AspectRatio     = m_AspectRatio;
    m_Camera.ImageWidth      = m_Width;
    m_Camera.SamplesPerPixel = m_SamplesPerPixel;
    m_Camera.MaxDepth        = m_MaxDepth;
    m_Camera.Background      = Render::Color3(0.70, 0.80, 1.00);

    m_Camera.Vfov     = 20;
    m_Camera.LookFrom = Math::Point3(13, 2, 3);
    m_Camera.LookAt   = Math::Point3(0, 0, 0);
    m_Camera.VUp      = Math::Vector3(0, 1, 0);

    m_Camera.DefocusAngle = 0.6;
    m_Camera.FocusDist    = 10.0;
}

}    // namespace Scenes
