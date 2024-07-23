#pragma once

#include "Color.h"
#include "Ray.h"

#include <Math/Vector3.h>

namespace Objects
{
class Hittable;
class HittableList;
}    // namespace Objects

namespace Render
{

class Camera
{
public:
    enum class RenderType : int
    {
        CPUOneCore   = 0,
        CPUMultiCore = 1,
        GPU          = 2
    };

    enum class SamplerType : int
    {
        Normal       = 0,
        Accumulation = 1,
        Stratified   = 2
    };

    double AspectRatio       = 1.0;    // Ratio of image width over height
    int    ImageWidth        = 100;    // Rendered image width in pixel count
    int    SamplesPerPixel   = 10;     // Count of random samples for each pixel
    double PixelSamplesScale = 1.0;    // Color scale factor for a sum of pixel samples
    int    SqrtSpp           = 3;      // Square root of number of samples per pixel
    double SqrtSppScale      = 1.0;    // 1.0 / SamplesPerPixel or 1.0 / SqrtSpp * SqrtSpp:
    int    MaxDepth          = 10;     // Maximum number of ray bounces into scene
    Color3 Background;                 // Scene background color

    RenderType  RenderingType = RenderType::CPUMultiCore;
    SamplerType SamplingType  = SamplerType::Stratified;

    bool UsePDF                 = true;    // Use probability density function for sampling
    bool UseUnidirectionalLight = true;    // Use unidirectional light sampling

    double        Vfov     = 90;                        // Vertical view angle (field of view)
    Math::Vector3 LookFrom = Math::Point3(0, 0, 0);     // Point camera is looking from
    Math::Point3  LookAt   = Math::Point3(0, 0, -1);    // Point camera is looking at
    Math::Vector3 VUp      = Math::Vector3(0, 1, 0);    // Camera-relative "up" direction

    double DefocusAngle = 0;     // Variation angle of rays through each pixel
    double FocusDist    = 10;    // Distance from camera lookfrom point to plane of perfect focus

    void Initialize();

    // Get RGBA color value for the pixel at location x,y.
    uint32_t GetPixel(uint32_t x, uint32_t y, const Objects::Hittable& world, const Objects::HittableList& lights) const;

    // Get a randomly-sampled camera ray for the pixel at location x,y, originating from the camera defocus disk.
    Ray GetRay(int x, int y, int xS = 0, int yS = 0) const;

    Color3 RayColorGradientBackground(const Ray& r, int depth, const Objects::Hittable& world) const;
    Color3 RayColor(const Ray& r, int depth, const Objects::Hittable& world, const Objects::HittableList& lights) const;

private:
    int           m_ImageHeight  = 0;    // Rendered image height
    double        m_RecipSqrtSpp = 1;    // 1 / sqrt_spp
    Math::Point3  m_Center;              // Camera m_center
    Math::Point3  m_Pixel00Loc;          // Location of pixel 0, 0
    Math::Vector3 m_PixelDeltaU;         // Offset to pixel to the right
    Math::Vector3 m_PixelDeltaV;         // Offset to pixel below
    Math::Vector3 m_U;                   // Camera frame basis vector U
    Math::Vector3 m_V;                   // Camera frame basis vector V
    Math::Vector3 m_W;                   // Camera frame basis vector W
    Math::Vector3 m_DefocusDiskU;        // Defocus disk horizontal radius
    Math::Vector3 m_DefocusDiskV;        // Defocus disk vertical radius

    Math::Point3 DefocusDiskSample() const;

    // Pixel Sampling Functions
    Math::Vector3 SampleSquare() const;
    Math::Vector3 SampleSquare(int xS, int yS) const;    // Stratified
    Math::Vector3 SampleDisk() const;
    Math::Vector3 SampleDisk(int xS, int yS) const;    // Stratified
};

}    // namespace Render
