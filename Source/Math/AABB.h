#pragma once

#include "Interval.h"
#include "Vector3.h"

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
    AABB(const Interval& x, const Interval& y, const Interval& z);

    // Treat the two points a and b as extrema for the bounding box, so we don't require a particular minimum/maximum coordinate order.
    AABB(const Point3& a, const Point3& b);
    AABB(const AABB& box0, const AABB& box1);

    // Return an AABB that has no side narrower than some delta, padding if necessary.
    AABB            Pad() const;
    const Interval& Axis(const int n) const;
    bool            HitNonOptimized(const Ray& ray, Interval rayT) const;
    bool            Hit(const Ray& ray, Interval rayT) const;

    // Returns the index of the longest axis of the bounding box.
    int LongestAxis() const;

    static const AABB Empty;
    static const AABB Universe;

private:
    // Adjust the AABB so that no side is narrower than some delta, padding if necessary.
    void PadToMinimums();
};

inline AABB operator+(const AABB& bbox, const Vector3& offset)
{
    return {bbox.X + offset.X(), bbox.Y + offset.Y(), bbox.Z + offset.Z()};
}

inline AABB operator+(const Vector3& offset, const AABB& bbox)
{
    return bbox + offset;
}
