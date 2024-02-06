#pragma once

#include <Objects/HittableList.h>
#include <Render/Camera.h>

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

    virtual Camera&       GetCamera();
    virtual Hittable&     GetWorld();
    virtual HittableList& GetLights();

protected:
    Camera       m_Camera;
    HittableList m_World;
    HittableList m_Lights;
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

class RTWeekOneTestScene final : public Scene
{
public:
    RTWeekOneTestScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
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

class RTWeekNextRandomSpheresScene final : public Scene
{
public:
    RTWeekNextRandomSpheresScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class RTWeekNextTwoSpheresScene final : public Scene
{
public:
    RTWeekNextTwoSpheresScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class RTWeekNextEarthScene final : public Scene
{
public:
    RTWeekNextEarthScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class RTWeekNextTwoPerlinSpheresScene final : public Scene
{
public:
    RTWeekNextTwoPerlinSpheresScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class RTWeekNextQuadsScene final : public Scene
{
public:
    RTWeekNextQuadsScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class RTWeekNextSimpleLightScene final : public Scene
{
public:
    RTWeekNextSimpleLightScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class RTWeekNextCornellBoxScene final : public Scene
{
public:
    RTWeekNextCornellBoxScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class RTWeekNextCornellSmokeScene final : public Scene
{
public:
    RTWeekNextCornellSmokeScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class RTWeekNextFinalScene final : public Scene
{
public:
    RTWeekNextFinalScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class RTWeekRestACornellBoxScene final : public Scene
{
public:
    RTWeekRestACornellBoxScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class RTWeekRestBCornellBoxMirrorScene final : public Scene
{
public:
    RTWeekRestBCornellBoxMirrorScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class RTWeekRestCCornellBoxGlassScene final : public Scene
{
public:
    RTWeekRestCCornellBoxGlassScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};

class CornellBoxLightsScene final : public Scene
{
public:
    CornellBoxLightsScene(double aspectRatio, int width, int samplesPerPixel, int maxDepth);
};
