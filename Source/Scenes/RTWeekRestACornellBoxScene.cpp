#include "Scene.h"

#include <Math/Vector3.h>
#include <Objects/Box.h>
#include <Objects/Quad.h>
#include <Objects/RotateY.h>
#include <Objects/Translate.h>
#include <Render/DiffuseLight.h>
#include <Render/Lambertian.h>

#include <memory>

namespace Scenes
{

RTWeekRestACornellBoxScene::RTWeekRestACornellBoxScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    auto red   = std::make_shared<Render::Lambertian>(Render::Color3(.65, .05, .05));
    auto white = std::make_shared<Render::Lambertian>(Render::Color3(.73, .73, .73));
    auto green = std::make_shared<Render::Lambertian>(Render::Color3(.12, .45, .15));
    auto light = std::make_shared<Render::DiffuseLight>(Render::Color3(15, 15, 15));

    // Cornell box sides
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(555, 0, 0), Math::Vector3(0, 0, 555), Math::Vector3(0, 555, 0), green));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(0, 0, 555), Math::Vector3(0, 0, -555), Math::Vector3(0, 555, 0), red));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(0, 555, 0), Math::Vector3(555, 0, 0), Math::Vector3(0, 0, 555), white));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(0, 0, 555), Math::Vector3(555, 0, 0), Math::Vector3(0, 0, -555), white));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(555, 0, 555), Math::Vector3(-555, 0, 0), Math::Vector3(0, 555, 0), white));

    // Lights
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(213, 554, 227), Math::Vector3(130, 0, 0), Math::Vector3(0, 0, 105), light));

    // Box 1
    std::shared_ptr<Objects::Hittable> box1 = Objects::Box(Math::Point3(0, 0, 0), Math::Point3(165, 330, 165), white);
    box1                                    = std::make_shared<Objects::RotateY>(box1, 15);
    box1                                    = std::make_shared<Objects::Translate>(box1, Math::Vector3(265, 0, 295));
    m_World.Add(box1);

    // Box 2
    std::shared_ptr<Objects::Hittable> box2 = Objects::Box(Math::Point3(0, 0, 0), Math::Point3(165, 165, 165), white);
    box2                                    = std::make_shared<Objects::RotateY>(box2, -18);
    box2                                    = std::make_shared<Objects::Translate>(box2, Math::Vector3(130, 0, 65));
    m_World.Add(box2);

    auto m = std::shared_ptr<Render::Material>();
    m_Lights.Add(std::make_shared<Objects::Quad>(Math::Point3(343, 554, 332), Math::Vector3(-130, 0, 0), Math::Vector3(0, 0, -105), m));

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
