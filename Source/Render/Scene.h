#pragma once

#include <Render/Camera.h>
#include <Render/HittableList.h>

class Scene
{
public:
    virtual ~Scene() = default;

    virtual Camera&   GetCamera() { return m_Camera; }
    virtual Hittable& GetWorld() { return m_World; }

protected:
    Camera       m_Camera;
    HittableList m_World;
};

class RtOneSceneWIP final : public Scene
{
public:
    RtOneSceneWIP(double aspectRatio, int width, int samplesPerPixel, int maxDepth);

private:
    double m_AspectRatio     = 16.0 / 9.0;
    int    m_Width           = 400;
    int    m_SamplesPerPixel = 1;
    int    m_MaxDepth        = 1;
};


class RtOneSceneFinal final : public Scene
{
public:
    RtOneSceneFinal(double aspectRatio, int width, int samplesPerPixel, int maxDepth);

private:
    double m_AspectRatio     = 16.0 / 9.0;
    int    m_Width           = 400;
    int    m_SamplesPerPixel = 1;
    int    m_MaxDepth        = 1;
};
