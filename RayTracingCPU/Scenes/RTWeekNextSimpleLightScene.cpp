#include "Scene.h"

#include <Math/Vector3.h>
#include <Objects/Quad.h>
#include <Objects/Sphere.h>
#include <Render/DiffuseLight.h>
#include <Render/Lambertian.h>
#include <Render/NoiseTexture.h>

#include <memory>

namespace Scenes
{

RTWeekNextSimpleLightScene::RTWeekNextSimpleLightScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    auto perlinTexture = std::make_shared<Render::NoiseTexture>(4);
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(0, -1000, 0), 1000, std::make_shared<Render::Lambertian>(perlinTexture)));
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(0, 2, 0), 2, std::make_shared<Render::Lambertian>(perlinTexture)));

    auto diffuseLight = std::make_shared<Render::DiffuseLight>(Render::Color3(4, 4, 4));
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(0, 7, 0), 2, diffuseLight));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(3, 1, -2), Math::Vector3(2, 0, 0), Math::Vector3(0, 2, 0), diffuseLight));

    auto lightMaterial = std::shared_ptr<Render::Material>();
    m_Lights.Add(std::make_shared<Objects::Sphere>(Math::Point3(0, 7, 0), 2, lightMaterial));
    m_Lights.Add(std::make_shared<Objects::Quad>(Math::Point3(3, 1, -2), Math::Vector3(2, 0, 0), Math::Vector3(0, 2, 0), lightMaterial));

    m_Camera.AspectRatio     = m_AspectRatio;
    m_Camera.ImageWidth      = m_Width;
    m_Camera.SamplesPerPixel = m_SamplesPerPixel;
    m_Camera.MaxDepth        = m_MaxDepth;
    m_Camera.Background      = Render::Color3(0, 0, 0);

    m_Camera.Vfov     = 20;
    m_Camera.LookFrom = Math::Point3(26, 3, 6);
    m_Camera.LookAt   = Math::Point3(0, 2, 0);
    m_Camera.VUp      = Math::Vector3(0, 1, 0);

    m_Camera.DefocusAngle = 0;
}

}    // namespace Scenes
