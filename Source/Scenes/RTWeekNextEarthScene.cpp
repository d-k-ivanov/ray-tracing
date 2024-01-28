#include "Scene.h"

#include <Math/Vector3.h>
#include <Render/BVH.h>
#include <Render/Material.h>
#include <Render/Sphere.h>
#include <Render/Texture.h>
#include <Utils/Filesystem.h>

#include <memory>

RTWeekNextEarthScene::RTWeekNextEarthScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    const auto earthTexturePath = ThisExecutableLocation() + "/Resources/Textures/earthmap.jpg";
    auto       earthTexture     = std::make_shared<ImageTexture>(earthTexturePath);
    auto       earthSurface     = std::make_shared<Lambertian>(earthTexture);
    m_World.Add(std::make_shared<Sphere>(Point3(0, 0, 0), 2, earthSurface));

    m_Camera.AspectRatio     = m_AspectRatio;
    m_Camera.ImageWidth      = m_Width;
    m_Camera.SamplesPerPixel = m_SamplesPerPixel;
    m_Camera.MaxDepth        = m_MaxDepth;

    m_Camera.Vfov = 20;

    m_Camera.LookFrom = Point3(13, 2, 3);
    m_Camera.LookAt   = Point3(0, 0, 0);
    m_Camera.VUp      = Vector3(0, 1, 0);

    m_Camera.DefocusAngle = 0;
}
