#include "Scene.h"

#include <Math/Vector3.h>
#include <Render/Material.h>
#include <Render/Quad.h>
#include <Render/Sphere.h>

#include <memory>

RTWeekNextSimpleLightScene::RTWeekNextSimpleLightScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    auto perlinTexture = std::make_shared<NoiseTexture>(4);
    m_World.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, std::make_shared<Lambertian>(perlinTexture)));
    m_World.Add(std::make_shared<Sphere>(Point3(0, 2, 0), 2, std::make_shared<Lambertian>(perlinTexture)));

    auto diffuseLight = std::make_shared<DiffuseLight>(Color3(4, 4, 4));
    m_World.Add(std::make_shared<Sphere>(Point3(0, 7, 0), 2, diffuseLight));
    m_World.Add(std::make_shared<Quad>(Point3(3, 1, -2), Vector3(2, 0, 0), Vector3(0, 2, 0), diffuseLight));

    auto lightMaterial = std::shared_ptr<Material>();
    m_Lights.Add(std::make_shared<Sphere>(Point3(0, 7, 0), 2, lightMaterial));
    m_Lights.Add(std::make_shared<Quad>(Point3(3, 1, -2), Vector3(2, 0, 0), Vector3(0, 2, 0), lightMaterial));

    m_Camera.AspectRatio     = m_AspectRatio;
    m_Camera.ImageWidth      = m_Width;
    m_Camera.SamplesPerPixel = m_SamplesPerPixel;
    m_Camera.MaxDepth        = m_MaxDepth;
    m_Camera.Background      = Color3(0, 0, 0);

    m_Camera.Vfov     = 20;
    m_Camera.LookFrom = Point3(26, 3, 6);
    m_Camera.LookAt   = Point3(0, 2, 0);
    m_Camera.VUp      = Vector3(0, 1, 0);

    m_Camera.DefocusAngle = 0;
}
