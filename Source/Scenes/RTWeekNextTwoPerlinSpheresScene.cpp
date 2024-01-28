#include "Scene.h"

#include <Math/Vector3.h>
#include <Render/Material.h>
#include <Render/Sphere.h>
#include <Render/Texture.h>

#include <memory>

RTWeekNextTwoPerlinSpheresScene::RTWeekNextTwoPerlinSpheresScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    // auto pertext = std::make_shared<NoiseTextureSmooth>(4);
    auto pertext = std::make_shared<NoiseTextureCamouflage>(4);
    // auto pertext = std::make_shared<NoiseTexture>(4);
    m_World.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, std::make_shared<Lambertian>(pertext)));
    m_World.Add(std::make_shared<Sphere>(Point3(0, 2, 0), 2, std::make_shared<Lambertian>(pertext)));

    m_Camera.AspectRatio     = m_AspectRatio;
    m_Camera.ImageWidth      = m_Width;
    m_Camera.SamplesPerPixel = m_SamplesPerPixel;
    m_Camera.MaxDepth        = m_MaxDepth;
    m_Camera.Background      = Color3(0.70, 0.80, 1.00);

    m_Camera.Vfov = 20;

    m_Camera.LookFrom = Point3(13, 2, 3);
    m_Camera.LookAt   = Point3(0, 0, 0);
    m_Camera.VUp      = Vector3(0, 1, 0);

    m_Camera.DefocusAngle = 0;
}
