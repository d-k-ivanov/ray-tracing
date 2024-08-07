#include "Scene.h"

#include <Math/Vector3.h>
#include <Objects/Quad.h>
#include <Render/Lambertian.h>

#include <memory>

namespace Scenes
{

RTWeekNextQuadsScene::RTWeekNextQuadsScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    // Materials
    auto leftRed     = std::make_shared<Render::Lambertian>(Render::Color3(1.0, 0.2, 0.2));
    auto backGreen   = std::make_shared<Render::Lambertian>(Render::Color3(0.2, 1.0, 0.2));
    auto rightBlue   = std::make_shared<Render::Lambertian>(Render::Color3(0.2, 0.2, 1.0));
    auto upperOrange = std::make_shared<Render::Lambertian>(Render::Color3(1.0, 0.5, 0.0));
    auto lowerTeal   = std::make_shared<Render::Lambertian>(Render::Color3(0.2, 0.8, 0.8));

    // Quads
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(-3, -2, 5), Math::Vector3(0, 0, -4), Math::Vector3(0, 4, 0), leftRed));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(-2, -2, 0), Math::Vector3(4, 0, 0), Math::Vector3(0, 4, 0), backGreen));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(3, -2, 1), Math::Vector3(0, 0, 4), Math::Vector3(0, 4, 0), rightBlue));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(-2, 3, 1), Math::Vector3(4, 0, 0), Math::Vector3(0, 0, 4), upperOrange));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(-2, -3, 5), Math::Vector3(4, 0, 0), Math::Vector3(0, 0, -4), lowerTeal));

    m_Camera.AspectRatio     = m_AspectRatio;
    m_Camera.ImageWidth      = m_Width;
    m_Camera.SamplesPerPixel = m_SamplesPerPixel;
    m_Camera.MaxDepth        = m_MaxDepth;
    m_Camera.Background      = Render::Color3(0.70, 0.80, 1.00);

    m_Camera.Vfov     = 80;
    m_Camera.LookFrom = Math::Point3(0, 0, 9);
    m_Camera.LookAt   = Math::Point3(0, 0, 0);
    m_Camera.VUp      = Math::Vector3(0, 1, 0);

    m_Camera.DefocusAngle = 0;
}

}    // namespace Scenes
