#include "Scene.h"

#include <Math/Vector3.h>
#include <Objects/Sphere.h>
#include <Render/Lambertian.h>
#include <Utils/Random.h>

namespace Scenes
{

RTWeekNextDefaultScene::RTWeekNextDefaultScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    auto materialGround = std::make_shared<Render::Lambertian>(Render::Color3(0.8, 0.8, 0.0));
    auto materialCenter = std::make_shared<Render::Lambertian>(Render::Color3(0.1, 0.2, 0.5));
    auto center         = Math::Point3(0.0, 0.0, -1.0);
    auto center2        = center + Math::Vector3(Utils::Random::Double(0.0, 0.5), 0, 0);

    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(0.0, -100.5, -1.0), 100.0, materialGround));
    m_World.Add(std::make_shared<Objects::Sphere>(center, center2, 0.5, materialCenter));

    m_Camera.AspectRatio     = m_AspectRatio;
    m_Camera.ImageWidth      = m_Width;
    m_Camera.SamplesPerPixel = m_SamplesPerPixel;
    m_Camera.MaxDepth        = m_MaxDepth;
    m_Camera.Background      = Render::Color3(0.70, 0.80, 1.00);

    m_Camera.DefocusAngle = 1.0;
    m_Camera.FocusDist    = 1.0;
}

}    // namespace Scenes
