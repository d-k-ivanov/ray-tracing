#include "Scene.h"

#include <Math/Vector3.h>
#include <Objects/Box.h>
#include <Objects/ConstantMedium.h>
#include <Objects/Quad.h>
#include <Objects/RotateY.h>
#include <Objects/Translate.h>
#include <Render/DiffuseLight.h>
#include <Render/Lambertian.h>

#include <memory>

RTWeekNextCornellSmokeScene::RTWeekNextCornellSmokeScene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
    : Scene(aspectRatio, width, samplesPerPixel, maxDepth)
{
    auto red   = std::make_shared<Lambertian>(Color3(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color3(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color3(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color3(7, 7, 7));

    m_World.Add(std::make_shared<Quad>(Point3(555, 0, 0), Vector3(0, 555, 0), Vector3(0, 0, 555), green));
    m_World.Add(std::make_shared<Quad>(Point3(0, 0, 0), Vector3(0, 555, 0), Vector3(0, 0, 555), red));
    m_World.Add(std::make_shared<Quad>(Point3(0, 555, 0), Vector3(555, 0, 0), Vector3(0, 0, 555), white));
    m_World.Add(std::make_shared<Quad>(Point3(0, 0, 0), Vector3(555, 0, 0), Vector3(0, 0, 555), white));
    m_World.Add(std::make_shared<Quad>(Point3(0, 0, 555), Vector3(555, 0, 0), Vector3(0, 555, 0), white));

    // Lights
    m_World.Add(std::make_shared<Quad>(Point3(113, 554, 127), Vector3(330, 0, 0), Vector3(0, 0, 305), light));
    m_Lights.Add(std::make_shared<Quad>(Point3(113, 554, 127), Vector3(329, 0, 0), Vector3(0, 0, 304), light));

    std::shared_ptr<Hittable> box1 = Box(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1                           = std::make_shared<RotateY>(box1, 15);
    box1                           = std::make_shared<Translate>(box1, Vector3(265, 0, 295));
    m_World.Add(std::make_shared<ConstantMedium>(box1, 0.01, Color3(0, 0, 0)));

    std::shared_ptr<Hittable> box2 = Box(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2                           = std::make_shared<RotateY>(box2, -18);
    box2                           = std::make_shared<Translate>(box2, Vector3(130, 0, 65));
    m_World.Add(std::make_shared<ConstantMedium>(box2, 0.01, Color3(1, 1, 1)));

    m_Camera.AspectRatio     = m_AspectRatio;
    m_Camera.ImageWidth      = m_Width;
    m_Camera.SamplesPerPixel = m_SamplesPerPixel;
    m_Camera.MaxDepth        = m_MaxDepth;
    m_Camera.Background      = Color3(0, 0, 0);

    m_Camera.Vfov     = 40;
    m_Camera.LookFrom = Point3(278, 278, -800);
    m_Camera.LookAt   = Point3(278, 278, 0);
    m_Camera.VUp      = Vector3(0, 1, 0);

    m_Camera.DefocusAngle = 0;
}
