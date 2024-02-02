// Axis-Aligned Bounding Boxes
#include "AABB.h"

AABB::AABB(const Interval& x, const Interval& y, const Interval& z)
    : X(x)
    , Y(y)
    , Z(z)
{
    PadToMinimums();
}

// Treat the two points a and b as extrema for the bounding box, so we don't require a particular minimum/maximum coordinate order.
AABB::AABB(const Point3& a, const Point3& b)
{
    X = Interval(fmin(a[0], b[0]), fmax(a[0], b[0]));
    Y = Interval(fmin(a[1], b[1]), fmax(a[1], b[1]));
    Z = Interval(fmin(a[2], b[2]), fmax(a[2], b[2]));

    PadToMinimums();
}

AABB::AABB(const AABB& box0, const AABB& box1)
{
    X = Interval(box0.X, box1.X);
    Y = Interval(box0.Y, box1.Y);
    Z = Interval(box0.Z, box1.Z);
}

// Return an AABB that has no side narrower than some delta, padding if necessary.
AABB AABB::Pad() const
{
    constexpr double delta = 0.0001;
    const Interval   newX  = (X.Size() >= delta) ? X : X.Expand(delta);
    const Interval   newY  = (Y.Size() >= delta) ? Y : Y.Expand(delta);
    const Interval   newZ  = (Z.Size() >= delta) ? Z : Z.Expand(delta);

    return {newX, newY, newZ};
}

const Interval& AABB::Axis(const int n) const
{
    if(n == 1)
        return Y;
    if(n == 2)
        return Z;
    return X;
}

bool AABB::HitNonOptimized(const Ray& ray, Interval rayT) const
{
    for(int a = 0; a < 3; a++)
    {
        const auto t0 = fmin(
            (Axis(a).Min - ray.Origin()[a]) / ray.Direction()[a],
            (Axis(a).Max - ray.Origin()[a]) / ray.Direction()[a]);
        const auto t1 = fmax(
            (Axis(a).Min - ray.Origin()[a]) / ray.Direction()[a],
            (Axis(a).Max - ray.Origin()[a]) / ray.Direction()[a]);

        rayT.Min = fmax(t0, rayT.Min);
        rayT.Max = fmin(t1, rayT.Max);

        if(rayT.Max <= rayT.Min)
            return false;
    }
    return true;
}

bool AABB::Hit(const Ray& ray, Interval rayT) const
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

// Returns the index of the longest axis of the bounding box.
int AABB::LongestAxis() const
{
    if(X.Size() > Y.Size())
    {
        return X.Size() > Y.Size() ? 0 : 2;
    }
    else
    {
        return Y.Size() > Z.Size() ? 1 : 2;
    }
}

// Adjust the AABB so that no side is narrower than some delta, padding if necessary.
void AABB::PadToMinimums()
{
    constexpr double delta = 0.0001;
    if(X.Size() < delta)
        X = X.Expand(delta);
    if(Y.Size() < delta)
        Y = Y.Expand(delta);
    if(Z.Size() < delta)
        Z = Z.Expand(delta);
}

const AABB AABB::Empty    = AABB(Interval::Empty, Interval::Empty, Interval::Empty);
const AABB AABB::Universe = AABB(Interval::Universe, Interval::Universe, Interval::Universe);
