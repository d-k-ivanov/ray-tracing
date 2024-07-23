#include "Scene.h"

#include <Math/Vector3.h>
#include <Objects/BVH.h>
#include <Objects/Box.h>
#include <Objects/ConstantMedium.h>
#include <Objects/Quad.h>
#include <Objects/RotateY.h>
#include <Objects/Sphere.h>
#include <Objects/Translate.h>
#include <Render/Dielectric.h>
#include <Render/DiffuseLight.h>
#include <Render/ImageTexture.h>
#include <Render/Lambertian.h>
#include <Render/Metal.h>
#include <Render/NoiseTexture.h>
#include <Utils/Filesystem.h>

#include <memory>

namespace Scenes
{

WhiteSperesScene::WhiteSperesScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    // Materials
    auto white   = std::make_shared<Render::Lambertian>(Render::Color3(.73, .73, .73));
    auto light   = std::make_shared<Render::DiffuseLight>(Render::Color3(7, 7, 7));
    auto nothing = std::shared_ptr<Render::Material>();

    // Lights
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(400, 554, 100), Math::Vector3(-300, 0, 0), Math::Vector3(0, 0, -100), light));
    m_Lights.Add(std::make_shared<Objects::Quad>(Math::Point3(400, 554, 100), Math::Vector3(-300, 0, 0), Math::Vector3(0, 0, -100), nothing));

    Objects::HittableList boxOfSpheres;
    constexpr int         ns = 20000;
    for(int j = 0; j < ns; j++)
    {
        boxOfSpheres.Add(std::make_shared<Objects::Sphere>(Math::Point3::Random(0, 500), 10, white));
    }

    m_World.Add(std::make_shared<Objects::Translate>(std::make_shared<Objects::RotateY>(std::make_shared<Objects::BVHNode>(boxOfSpheres), 15), Math::Vector3(0, 0, 300)));

    m_Camera.AspectRatio     = m_AspectRatio;
    m_Camera.ImageWidth      = m_Width;
    m_Camera.SamplesPerPixel = m_SamplesPerPixel;
    m_Camera.MaxDepth        = m_MaxDepth;
    m_Camera.Background      = Render::Color3(0, 0, 0);

    m_Camera.Vfov     = 40;
    m_Camera.LookFrom = Math::Point3(278, 278, -800);
    m_Camera.LookAt   = Math::Point3(278, 278, 0);
    m_Camera.VUp      = Math::Vector3(0, 1, 0);

    m_Camera.DefocusAngle = 0;
}

}    // namespace Scenes
