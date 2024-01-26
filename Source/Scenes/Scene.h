#pragma once

#include <Render/Camera.h>
#include <Render/HittableList.h>

class Scene
{
public:
    Scene(const double aspectRatio, const int width, const int samplesPerPixel, const int maxDepth)
        : m_AspectRatio(aspectRatio)
        , m_Width(width)
        , m_SamplesPerPixel(samplesPerPixel)
        , m_MaxDepth(maxDepth)
    {
    }
    virtual ~Scene() = default;

    virtual Camera&   GetCamera() { return m_Camera; }
    virtual Hittable& GetWorld() { return m_World; }

protected:
    Camera       m_Camera;
    HittableList m_World;
    double       m_AspectRatio     = 16.0 / 9.0;
    int          m_Width           = 400;
    int          m_SamplesPerPixel = 1;
    int          m_MaxDepth        = 1;
};

class RTWeekOneDefaultScene final : public Scene
{
public:
    RTWeekOneDefaultScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class RTWeekOneScene final : public Scene
{
public:
    RTWeekOneScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class RTWeekOneFinalScene final : public Scene
{
public:
    RTWeekOneFinalScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class RTWeekNextDefaultScene final : public Scene
{
public:
    RTWeekNextDefaultScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};


class RTWeekNextScene final : public Scene
{
public:
    RTWeekNextScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};
