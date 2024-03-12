#pragma once

#include "Color.h"
#include "Ray.h"

#include <Math/Vector3.h>
#include <Objects/Hittable.h>

class Camera
{
public:
    double AspectRatio     = 1.0;    // Ratio of image width over height
    int    ImageWidth      = 100;    // Rendered image width in pixel count
    int    SamplesPerPixel = 10;     // Count of random samples for each pixel
    double PixelSampleScale;         // Count of random samples for each pixel
    int    SqrtSpp = 3;              // Square root of number of samples per pixel
    double SqrtSppScale;             // 1.0 / SamplesPerPixel or 1.0 / SqrtSpp * SqrtSpp:
    int    MaxDepth = 10;            // Maximum number of ray bounces into scene
    Color3 Background;               // Scene background color

    bool UsePDF                 = true;
    bool UseUnidirectionalLight = true;

    double  Vfov     = 90;                  // Vertical view angle (field of view)
    Point3  LookFrom = Point3(0, 0, 0);     // Point camera is looking from
    Point3  LookAt   = Point3(0, 0, -1);    // Point camera is looking at
    Vector3 VUp      = Vector3(0, 1, 0);    // Camera-relative "up" direction

    double DefocusAngle = 0;     // Variation angle of rays through each pixel
    double FocusDist    = 10;    // Distance from camera lookfrom point to plane of perfect focus

    void Initialize();

    // Get a randomly-sampled camera ray for the pixel at location i,j, originating from the camera defocus disk.
    Ray GetRay(int i, int j) const;

    // Get a randomly-sampled camera ray for the pixel at location i,j,
    // originating from the camera defocus disk, and randomly sampled around the pixel location.
    Ray GetRay(int i, int j, int iS, int jS) const;

    Color3 RayColorGradientBackground(const Ray& r, int depth, const Hittable& world) const;
    Color3 RayColor(const Ray& r, int depth, const Hittable& world) const;
    Color3 RayColor(const Ray& r, int depth, const Hittable& world, const Hittable& lights) const;

private:
    int     m_ImageHeight  = 0;    // Rendered image height
    double  m_RecipSqrtSpp = 1;    // 1 / sqrt_spp
    Point3  m_Center;              // Camera m_center
    Point3  m_Pixel00Loc;          // Location of pixel 0, 0
    Vector3 m_PixelDeltaU;         // Offset to pixel to the right
    Vector3 m_PixelDeltaV;         // Offset to pixel below
    Vector3 m_U;                   // Camera frame basis vectors
    Vector3 m_V;                   // Camera frame basis vectors
    Vector3 m_W;                   // Camera frame basis vectors
    Vector3 m_DefocusDiskU;        // Defocus disk horizontal radius
    Vector3 m_DefocusDiskV;        // Defocus disk vertical radius

    // Returns a random point in the square surrounding a pixel at the origin.
    Vector3 PixelSampleSquare() const;

    // Returns a random point in the square surrounding a pixel at the origin, given the two subpixel indices.
    Vector3 PixelSampleSquare(int iS, int jS) const;

    // Generate a sample from the disk of given radius around a pixel at the origin.
    Vector3 PixelSampleDisk(double radius) const;

    // Returns a random point in the camera defocus disk.
    Point3 DefocusDiskSample() const;
};
