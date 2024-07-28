#include "SceneList.h"

#include <Objects/Model.h>
#include <Render/Material.h>
#include <Render/Texture.h>
#include <Utils/Filesystem.h>
#include <Utils/Random.h>

namespace
{

// Common models from the final scene from Ray Tracing In One Weekend book. Only the three central spheres are missing.
void AddRayTracingInOneWeekendCommonScene(std::vector<Objects::Model>& models, const bool& isProcedural)
{

    models.push_back(Objects::Model::CreateSphere(glm::vec3(0, -1000, 0), 1000, Render::Material::Lambertian(glm::vec3(0.5f, 0.5f, 0.5f)), isProcedural));

    for(int i = -11; i < 11; ++i)
    {
        for(int j = -11; j < 11; ++j)
        {
            const float     chooseMat = Utils::Random::Float();
            const float     centerY   = static_cast<float>(j) + 0.9f * Utils::Random::Float();
            const float     centerX   = static_cast<float>(i) + 0.9f * Utils::Random::Float();
            const glm::vec3 center(centerX, 0.2f, centerY);

            if(length(center - glm::vec3(4, 0.2f, 0)) > 0.9f)
            {
                if(chooseMat < 0.8f)    // Diffuse
                {
                    const float b = Utils::Random::Float() * Utils::Random::Float();
                    const float g = Utils::Random::Float() * Utils::Random::Float();
                    const float r = Utils::Random::Float() * Utils::Random::Float();

                    models.push_back(Objects::Model::CreateSphere(center, 0.2f, Render::Material::Lambertian(glm::vec3(r, g, b)), isProcedural));
                }
                else if(chooseMat < 0.95f)    // Metal
                {
                    const float fuzziness = 0.5f * Utils::Random::Float();
                    const float b         = 0.5f * (1 + Utils::Random::Float());
                    const float g         = 0.5f * (1 + Utils::Random::Float());
                    const float r         = 0.5f * (1 + Utils::Random::Float());

                    models.push_back(Objects::Model::CreateSphere(center, 0.2f, Render::Material::Metallic(glm::vec3(r, g, b), fuzziness), isProcedural));
                }
                else    // Glass
                {
                    models.push_back(Objects::Model::CreateSphere(center, 0.2f, Render::Material::Dielectric(1.5f), isProcedural));
                }
            }
        }
    }
}

}    // namespace

namespace Scenes
{

const std::vector<std::pair<std::string, std::function<SceneAssets(SceneList::CameraInitialSate&)>>> SceneList::AllScenes =
    {
        {"Cube And Spheres", CubeAndSpheres},
        {"Ray Tracing In One Weekend", RayTracingInOneWeekend},
        {"Planets In One Weekend", PlanetsInOneWeekend},
        {"Lucy In One Weekend", LucyInOneWeekend},
        {"Cornell Box", CornellBox},
        {"Cornell Box & Lucy", CornellBoxLucy},
};

SceneAssets SceneList::CubeAndSpheres(CameraInitialSate& camera)
{
    // Basic test scene.

    camera.ModelView       = translate(glm::mat4(1), glm::vec3(0, 0, -2));
    camera.FieldOfView     = 90;
    camera.Aperture        = 0.05f;
    camera.FocusDistance   = 2.0f;
    camera.ControlSpeed    = 2.0f;
    camera.GammaCorrection = false;
    camera.HasSky          = true;

    std::vector<Objects::Model>  models;
    std::vector<Render::Texture> textures;

    models.push_back(Objects::Model::LoadModel(Utils::ThisExecutableLocation() + "/Resources/Models/cube_multi.obj"));
    models.push_back(Objects::Model::CreateSphere(glm::vec3(1, 0, 0), 0.5, Render::Material::Metallic(glm::vec3(0.7f, 0.5f, 0.8f), 0.2f), true));
    models.push_back(Objects::Model::CreateSphere(glm::vec3(-1, 0, 0), 0.5, Render::Material::Dielectric(1.5f), true));
    models.push_back(Objects::Model::CreateSphere(glm::vec3(0, 1, 0), 0.5, Render::Material::Lambertian(glm::vec3(1.0f), 0), true));

    textures.push_back(Render::Texture::LoadTexture(Utils::ThisExecutableLocation() + "/Resources/Textures/earth_daymap.jpg", Engine::SamplerConfig()));

    return std::forward_as_tuple(std::move(models), std::move(textures));
}

// Final scene from Ray Tracing In One Weekend book.
SceneAssets SceneList::RayTracingInOneWeekend(CameraInitialSate& camera)
{

    camera.ModelView       = lookAt(glm::vec3(13, 2, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera.FieldOfView     = 20;
    camera.Aperture        = 0.1f;
    camera.FocusDistance   = 10.0f;
    camera.ControlSpeed    = 5.0f;
    camera.GammaCorrection = true;
    camera.HasSky          = true;

    constexpr bool isProcedural = true;

    std::vector<Objects::Model> models;

    AddRayTracingInOneWeekendCommonScene(models, isProcedural);

    models.push_back(Objects::Model::CreateSphere(glm::vec3(0, 1, 0), 1.0f, Render::Material::Dielectric(1.5f), isProcedural));
    models.push_back(Objects::Model::CreateSphere(glm::vec3(-4, 1, 0), 1.0f, Render::Material::Lambertian(glm::vec3(0.4f, 0.2f, 0.1f)), isProcedural));
    models.push_back(Objects::Model::CreateSphere(glm::vec3(4, 1, 0), 1.0f, Render::Material::Metallic(glm::vec3(0.7f, 0.6f, 0.5f), 0.0f), isProcedural));

    return std::forward_as_tuple(std::move(models), std::vector<Render::Texture>());
}

// Same as RayTracingInOneWeekend but using textures.
SceneAssets SceneList::PlanetsInOneWeekend(CameraInitialSate& camera)
{

    camera.ModelView       = lookAt(glm::vec3(13, 2, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera.FieldOfView     = 20;
    camera.Aperture        = 0.1f;
    camera.FocusDistance   = 10.0f;
    camera.ControlSpeed    = 5.0f;
    camera.GammaCorrection = true;
    camera.HasSky          = true;

    constexpr bool isProcedural = true;

    std::vector<Objects::Model>  models;
    std::vector<Render::Texture> textures;

    AddRayTracingInOneWeekendCommonScene(models, isProcedural);

    // Venus
    models.push_back(Objects::Model::CreateSphere(glm::vec3(4, 1, 0), 1.0f, Render::Material::Lambertian(glm::vec3(1.0f), 0), isProcedural));
    // Earth
    models.push_back(Objects::Model::CreateSphere(glm::vec3(0, 1, 0), 1.0f, Render::Material::Lambertian(glm::vec3(1.0f), 1), isProcedural));
    // Mars
    models.push_back(Objects::Model::CreateSphere(glm::vec3(-4, 1, 0), 1.0f, Render::Material::Lambertian(glm::vec3(1.0f), 2), isProcedural));

    textures.push_back(Render::Texture::LoadTexture(Utils::ThisExecutableLocation() + "/Resources/Textures/venus_atmosphere.jpg", Engine::SamplerConfig()));
    textures.push_back(Render::Texture::LoadTexture(Utils::ThisExecutableLocation() + "/Resources/Textures/earth_nightmap.jpg", Engine::SamplerConfig()));
    textures.push_back(Render::Texture::LoadTexture(Utils::ThisExecutableLocation() + "/Resources/Textures/mars.jpg", Engine::SamplerConfig()));

    return std::forward_as_tuple(std::move(models), std::move(textures));
}

// Same as RayTracingInOneWeekend but using the Lucy 3D model.
SceneAssets SceneList::LucyInOneWeekend(CameraInitialSate& camera)
{

    camera.ModelView       = lookAt(glm::vec3(13, 2, 3), glm::vec3(0, 1.0, 0), glm::vec3(0, 1, 0));
    camera.FieldOfView     = 20;
    camera.Aperture        = 0.05f;
    camera.FocusDistance   = 10.0f;
    camera.ControlSpeed    = 5.0f;
    camera.GammaCorrection = true;
    camera.HasSky          = true;

    constexpr bool isProcedural = true;

    std::vector<Objects::Model> models;

    AddRayTracingInOneWeekendCommonScene(models, isProcedural);

    auto lucy0 = Objects::Model::LoadModel(Utils::ThisExecutableLocation() + "/Resources/Models/lucy.obj");
    auto lucy1 = lucy0;
    auto lucy2 = lucy0;

    constexpr auto  i           = glm::mat4(1);
    constexpr float scaleFactor = 0.0035f;

    lucy0.Transform(
        rotate(
            scale(
                translate(i, glm::vec3(0, -0.08f, 0)),
                glm::vec3(scaleFactor)),
            glm::radians(90.0f), glm::vec3(0, 1, 0)));

    lucy1.Transform(
        rotate(
            scale(
                translate(i, glm::vec3(-4, -0.08f, 0)),
                glm::vec3(scaleFactor)),
            glm::radians(90.0f), glm::vec3(0, 1, 0)));

    lucy2.Transform(
        rotate(
            scale(
                translate(i, glm::vec3(4, -0.08f, 0)),
                glm::vec3(scaleFactor)),
            glm::radians(90.0f), glm::vec3(0, 1, 0)));

    lucy0.SetMaterial(Render::Material::Dielectric(1.5f));
    lucy1.SetMaterial(Render::Material::Lambertian(glm::vec3(0.4f, 0.2f, 0.1f)));
    lucy2.SetMaterial(Render::Material::Metallic(glm::vec3(0.7f, 0.6f, 0.5f), 0.05f));

    models.push_back(std::move(lucy0));
    models.push_back(std::move(lucy1));
    models.push_back(std::move(lucy2));

    return std::forward_as_tuple(std::move(models), std::vector<Render::Texture>());
}

SceneAssets SceneList::CornellBox(CameraInitialSate& camera)
{
    camera.ModelView       = lookAt(glm::vec3(278, 278, 800), glm::vec3(278, 278, 0), glm::vec3(0, 1, 0));
    camera.FieldOfView     = 40;
    camera.Aperture        = 0.0f;
    camera.FocusDistance   = 10.0f;
    camera.ControlSpeed    = 500.0f;
    camera.GammaCorrection = true;
    camera.HasSky          = false;

    constexpr auto i     = glm::mat4(1);
    const auto     white = Render::Material::Lambertian(glm::vec3(0.73f, 0.73f, 0.73f));

    auto box0 = Objects::Model::CreateBox(glm::vec3(0, 0, -165), glm::vec3(165, 165, 0), white);
    auto box1 = Objects::Model::CreateBox(glm::vec3(0, 0, -165), glm::vec3(165, 330, 0), white);

    box0.Transform(rotate(translate(i, glm::vec3(555 - 130 - 165, 0, -65)), glm::radians(-18.0f), glm::vec3(0, 1, 0)));
    box1.Transform(rotate(translate(i, glm::vec3(555 - 265 - 165, 0, -295)), glm::radians(15.0f), glm::vec3(0, 1, 0)));

    std::vector<Objects::Model> models;
    models.push_back(Objects::Model::CreateCornellBox(555));
    models.push_back(box0);
    models.push_back(box1);

    return std::make_tuple(std::move(models), std::vector<Render::Texture>());
}

SceneAssets SceneList::CornellBoxLucy(CameraInitialSate& camera)
{
    camera.ModelView       = lookAt(glm::vec3(278, 278, 800), glm::vec3(278, 278, 0), glm::vec3(0, 1, 0));
    camera.FieldOfView     = 40;
    camera.Aperture        = 0.0f;
    camera.FocusDistance   = 10.0f;
    camera.ControlSpeed    = 500.0f;
    camera.GammaCorrection = true;
    camera.HasSky          = false;

    constexpr auto i      = glm::mat4(1);
    const auto     sphere = Objects::Model::CreateSphere(glm::vec3(555 - 130, 165.0f, -165.0f / 2 - 65), 80.0f, Render::Material::Dielectric(1.5f), true);
    auto           lucy0  = Objects::Model::LoadModel(Utils::ThisExecutableLocation() + "/Resources/Models/lucy.obj");

    lucy0.Transform(
        rotate(
            scale(
                translate(i, glm::vec3(555 - 300 - 165 / 2, -9, -295 - 165 / 2)),
                glm::vec3(0.6f)),
            glm::radians(75.0f), glm::vec3(0, 1, 0)));

    std::vector<Objects::Model> models;
    models.push_back(Objects::Model::CreateCornellBox(555));
    models.push_back(sphere);
    models.push_back(lucy0);

    return std::forward_as_tuple(std::move(models), std::vector<Render::Texture>());
}

}    // namespace Scenes
