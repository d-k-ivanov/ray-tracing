#include "Scene.h"

namespace Scenes
{

Render::Camera& Scene::GetCamera()
{
    return m_Camera;
}

Objects::Hittable& Scene::GetWorld()
{
    return m_World;
}

Objects::HittableList& Scene::GetLights()
{
    return m_Lights;
}

}    // namespace Scenes
