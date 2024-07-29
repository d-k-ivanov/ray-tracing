#pragma once
#include <Utils/Glm.h>

#include <functional>
#include <string>
#include <tuple>
#include <vector>

namespace Objects
{
class Model;
}

namespace Render
{
class Texture;
}

namespace Scenes
{

typedef std::tuple<std::vector<Objects::Model>, std::vector<Render::Texture>> SceneAssets;

class SceneList final
{
public:
    struct CameraInitialSate
    {
        glm::mat4 ModelView;
        float     FieldOfView;
        float     Aperture;
        float     FocusDistance;
        float     ControlSpeed;
        bool      GammaCorrection;
        bool      HasSky;
    };

    static SceneAssets CubeAndSpheres(CameraInitialSate& camera);
    static SceneAssets RayTracingInOneWeekend(CameraInitialSate& camera);
    static SceneAssets PlanetsInOneWeekend(CameraInitialSate& camera);
    static SceneAssets LucyInOneWeekend(CameraInitialSate& camera);
    static SceneAssets CornellBox(CameraInitialSate& camera);
    static SceneAssets CornellBoxLucy(CameraInitialSate& camera);
    static SceneAssets CornellBoxLights(CameraInitialSate& camera);

    static const std::vector<std::pair<std::string, std::function<SceneAssets(CameraInitialSate&)>>> AllScenes;
};

}    // namespace Scenes
