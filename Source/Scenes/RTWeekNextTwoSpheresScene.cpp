#include "Scene.h"

#include <Math/Vector3.h>
#include <Objects/Sphere.h>
#include <Render/CheckerTexture.h>
#include <Render/Lambertian.h>

#include <memory>

namespace Scenes
{

RTWeekNextTwoSpheresScene::RTWeekNextTwoSpheresScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    auto checker = std::make_shared<Render::CheckerTexture>(0.32, Render::Color3(.2, .3, .1), Render::Color3(.9, .9, .9));

    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(0, -10, 0), 10, std::make_shared<Render::Lambertian>(checker)));
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(0, 10, 0), 10, std::make_shared<Render::Lambertian>(checker)));

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
