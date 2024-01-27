#pragma once

#include <Math/Interval.h>
#include <Math/Vector3.h>
#include <Render/Ray.h>

// Axis-Aligned Bounding Boxes
class AABB
{
public:
    Interval X;
    Interval Y;
    Interval Z;

    // The default AABB is empty, since Intervals are empty by default.
    AABB() = default;

    AABB(const Interval& ix, const Interval& iy, const Interval& iz)
        : X(ix)
        , Y(iy)
        , Z(iz)
    {
    }

    AABB(const Point3& a, const Point3& b)
    {
        // Treat the two points a and b as extrema for the bounding box, so we don't require a
        // particular minimum/maximum coordinate order.
        X = Interval(fmin(a[0], b[0]), fmax(a[0], b[0]));
        Y = Interval(fmin(a[1], b[1]), fmax(a[1], b[1]));
        Z = Interval(fmin(a[2], b[2]), fmax(a[2], b[2]));
    }

    AABB(const AABB& box0, const AABB& box1)
    {
        X = Interval(box0.X, box1.X);
        Y = Interval(box0.Y, box1.Y);
        Z = Interval(box0.Z, box1.Z);
    }

    // Return an AABB that has no side narrower than some delta, padding if necessary.
    AABB Pad() const
    {
        constexpr double delta = 0.0001;
        const Interval   newX  = (X.Size() >= delta) ? X : X.Expand(delta);
        const Interval   newY  = (Y.Size() >= delta) ? Y : Y.Expand(delta);
        const Interval   newZ  = (Z.Size() >= delta) ? Z : Z.Expand(delta);

        return {newX, newY, newZ};
    }

    const Interval& Axis(const int n) const
    {
        if(n == 1)
            return Y;
        if(n == 2)
            return Z;
        return X;
    }

    bool HitNonOptimized(const Ray& ray, Interval rayT) const
    {
        for(int a = 0; a < 3; a++)
        {
            const auto t0 = fmin(
                (Axis(a).Min - ray.Origin()[a]) / ray.Direction()[a],
                (Axis(a).Max - ray.Origin()[a]) / ray.Direction()[a]
            );
            const auto t1 = fmax(
                (Axis(a).Min - ray.Origin()[a]) / ray.Direction()[a],
                (Axis(a).Max - ray.Origin()[a]) / ray.Direction()[a]
            );

            rayT.Min = fmax(t0, rayT.Min);
            rayT.Max = fmin(t1, rayT.Max);

            if(rayT.Max <= rayT.Min)
                return false;
        }
        return true;
    }

    bool Hit(const Ray& ray, Interval rayT) const
    {
        for(int a = 0; a < 3; a++)
        {
            const auto invD = 1 / ray.Direction()[a];
            const auto orig = ray.Origin()[a];

            auto t0 = (Axis(a).Min - orig) * invD;
            auto t1 = (Axis(a).Max - orig) * invD;

            if(invD < 0)
                std::swap(t0, t1);

            if(t0 > rayT.Min)
                rayT.Min = t0;
            if(t1 < rayT.Max)
                rayT.Max = t1;

            if(rayT.Max <= rayT.Min)
                return false;
        }
        return true;
    }
};

inline AABB operator+(const AABB& bbox, const Vector3& offset)
{
    return {bbox.X + offset.X(), bbox.Y + offset.Y(), bbox.Z + offset.Z()};
}

inline AABB operator+(const Vector3& offset, const AABB& bbox)
{
    return bbox + offset;
}
