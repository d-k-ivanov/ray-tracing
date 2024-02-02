#include "HitRecord.h"

#include "Ray.h"

void HitRecord::SetFaceNormal(const Ray& r, const Vector3& outwardNormal)
{
    FrontFace = DotProduct(r.Direction(), outwardNormal) < 0;
    Normal    = FrontFace ? outwardNormal : -outwardNormal;
}