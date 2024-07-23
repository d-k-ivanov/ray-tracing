#include "Scene.h"

#include <Math/Vector3.h>
#include <Objects/Sphere.h>
#include <Render/Lambertian.h>
#include <Render/NoiseTexture.h>
#include <Render/NoiseTextureCamouflage.h>
#include <Render/NoiseTextureSmooth.h>

#include <memory>

namespace Scenes
{

RTWeekNextTwoPerlinSpheresScene::RTWeekNextTwoPerlinSpheresScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    auto pertextSmooth = std::make_shared<Render::NoiseTextureSmooth>(4);
    auto pertextCamou  = std::make_shared<Render::NoiseTextureCamouflage>(4);
    auto pertextNoise  = std::make_shared<Render::NoiseTexture>(4);
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(0, -1000, 0), 1000, std::make_shared<Render::Lambertian>(pertextCamou)));
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(0, 2, 0), 2, std::make_shared<Render::Lambertian>(pertextNoise)));

    m_Camera.AspectRatio     = m_AspectRatio;
    m_Camera.ImageWidth      = m_Width;
    m_Camera.SamplesPerPixel = m_SamplesPerPixel;
    m_Camera.MaxDepth        = m_MaxDepth;
    m_Camera.Background      = Render::Color3(0.70, 0.80, 1.00);

    m_Camera.Vfov = 20;

    m_Camera.LookFrom = Math::Point3(13, 2, 3);
    m_Camera.LookAt   = Math::Point3(0, 0, 0);
    m_Camera.VUp      = Math::Vector3(0, 1, 0);

    m_Camera.DefocusAngle = 0;
}

}    // namespace Scenes
