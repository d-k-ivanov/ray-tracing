#pragma once

#include "Color.h"
#include "Ray.h"

#include <memory>

namespace Math
{
class PDF;
}

namespace Render
{

class ScatterRecord
{
public:
    Color3                     Attenuation;
    std::shared_ptr<Math::PDF> PDFPtr;
    bool                       SkipPDF = false;
    Ray                        SkipPDFRay;
};

}    // namespace Render
