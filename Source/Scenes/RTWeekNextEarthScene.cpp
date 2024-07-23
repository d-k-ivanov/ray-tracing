#include "Scene.h"

#include <Math/Vector3.h>
#include <Objects/Sphere.h>
#include <Render/ImageTexture.h>
#include <Render/Lambertian.h>
#include <Utils/Filesystem.h>

#include <memory>

namespace Scenes
{

RTWeekNextEarthScene::RTWeekNextEarthScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    const auto earthTexturePath = Utils::ThisExecutableLocation() + "/Resources/Textures/earthmap.jpg";
    auto       earthTexture     = std::make_shared<Render::ImageTexture>(earthTexturePath);
    auto       earthSurface     = std::make_shared<Render::Lambertian>(earthTexture);
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(0, 0, 0), 2, earthSurface));

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
