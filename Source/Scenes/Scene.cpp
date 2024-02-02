#include "Scene.h"

Camera& Scene::GetCamera()
{
    return m_Camera;
}

Hittable& Scene::GetWorld()
{
    return m_World;
}

HittableList& Scene::GetLights()
{
    return m_Lights;
}
