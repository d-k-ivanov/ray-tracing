#include "Scene.h"

#include <Math/Vector3.h>
#include <Objects/Box.h>
#include <Objects/ConstantMedium.h>
#include <Objects/Quad.h>
#include <Objects/RotateY.h>
#include <Objects/Sphere.h>
#include <Objects/Translate.h>
#include <Render/Dielectric.h>
#include <Render/DiffuseLight.h>
#include <Render/Lambertian.h>
#include <Render/Metal.h>

#include <memory>

namespace Scenes
{

CornellBoxLightsScene::CornellBoxLightsScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    // Materials
    auto red   = std::make_shared<Render::Lambertian>(Render::Color3(.65, .05, .05));
    auto white = std::make_shared<Render::Lambertian>(Render::Color3(.73, .73, .73));
    auto green = std::make_shared<Render::Lambertian>(Render::Color3(.12, .45, .15));

    auto lightR   = std::make_shared<Render::DiffuseLight>(Render::Color3(20, 0.7, 0.7));
    auto lightG   = std::make_shared<Render::DiffuseLight>(Render::Color3(0.7, 20, 0.7));
    auto lightB   = std::make_shared<Render::DiffuseLight>(Render::Color3(0.7, 0.7, 20));
    auto lightW   = std::make_shared<Render::DiffuseLight>(Render::Color3(15, 15, 15));
    auto glass    = std::make_shared<Render::Dielectric>(1.5);
    auto aluminum = std::make_shared<Render::Metal>(Render::Color3(0.8, 0.85, 0.88), 0.0);
    auto nullMat  = std::shared_ptr<Render::Material>();

    // Cornell box sides
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(555, 0, 0), Math::Vector3(0, 0, 555), Math::Vector3(0, 555, 0), green));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(0, 0, 555), Math::Vector3(0, 0, -555), Math::Vector3(0, 555, 0), red));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(0, 555, 0), Math::Vector3(555, 0, 0), Math::Vector3(0, 0, 555), white));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(0, 0, 555), Math::Vector3(555, 0, 0), Math::Vector3(0, 0, -555), white));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(555, 0, 555), Math::Vector3(-555, 0, 0), Math::Vector3(0, 555, 0), white));

    // Lights

    // m_World.Add(std::make_shared<Objects:Quad>(Math::Point3(455, 50, 554), Math::Vector3(-355, 0, 0), Math::Vector3(0, 10, 0), lightW));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(455, 100, 554), Math::Vector3(-355, 0, 0), Math::Vector3(0, 10, 0), lightW));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(455, 150, 554), Math::Vector3(-355, 0, 0), Math::Vector3(0, 10, 0), lightW));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(455, 200, 554), Math::Vector3(-355, 0, 0), Math::Vector3(0, 10, 0), lightW));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(455, 250, 554), Math::Vector3(-355, 0, 0), Math::Vector3(0, 10, 0), lightW));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(455, 300, 554), Math::Vector3(-355, 0, 0), Math::Vector3(0, 10, 0), lightW));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(455, 350, 554), Math::Vector3(-355, 0, 0), Math::Vector3(0, 10, 0), lightW));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(455, 400, 554), Math::Vector3(-355, 0, 0), Math::Vector3(0, 10, 0), lightW));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(455, 450, 554), Math::Vector3(-355, 0, 0), Math::Vector3(0, 10, 0), lightW));
    m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(455, 500, 554), Math::Vector3(-355, 0, 0), Math::Vector3(0, 10, 0), lightW));
    // m_World.Add(std::make_shared<Objects:Quad>(Math::Point3(555, 0, 555), Math::Vector3(-555, 0, 0), Math::Vector3(0, 555, 0), white));

    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(530, 530, 277), 50, lightR));
    m_Lights.Add(std::make_shared<Objects::Sphere>(Math::Point3(530, 530, 277), 50, lightR));
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(277, 530, 277), 50, lightB));
    m_Lights.Add(std::make_shared<Objects::Sphere>(Math::Point3(277, 530, 277), 50, lightB));
    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(30, 530, 277), 50, lightG));
    m_Lights.Add(std::make_shared<Objects::Sphere>(Math::Point3(30, 530, 277), 50, lightG));
    // m_World.Add(std::make_shared<Objects::Sphere>(Point3(277, 25, 277), 50, lightW));

    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(405, 100, 277.5), 100, glass));
    m_Lights.Add(std::make_shared<Objects::Sphere>(Math::Point3(405, 100, 277.5), 100, nullMat));

    m_World.Add(std::make_shared<Objects::Sphere>(Math::Point3(150, 100, 277.5), 100, white));

    // m_World.Add(std::make_shared<Objects::Quad>(Math::Point3(213, 1, 227), Math::Vector3(0, 0, 105), Math::Vector3(130, 0, 0), lightW));
    // m_Lights.Add(std::make_shared<Objects::Quad>(Math::Point3(213, 2, 227), Math::Vector3(0, 0, 105), Math::Vector3(130, 0, 0), nullMat));

    // Box 1
    // std::shared_ptr<Hittable> box1 = Box(Point3(0, 0, 0), Point3(165, 330, 165), white);
    // box1                           = std::make_shared<RotateY>(box1, 15);
    // box1                           = std::make_shared<Translate>(box1, Vector3(265, 0, 295));
    // m_World.Add(box1);

    // Box 2
    // std::shared_ptr<Hittable> box2 = Box(Point3(0, 0, 0), Point3(165, 165, 165), white);
    // box2                           = std::make_shared<RotateY>(box2, -18);
    // box2                           = std::make_shared<Translate>(box2, Vector3(130, 0, 65));
    // m_World.Add(box2);

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
