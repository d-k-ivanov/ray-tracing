#pragma once

#include "Color.h"
#include "Ray.h"

#include <memory>

class PDF;

class ScatterRecord
{
public:
    Color3               Attenuation;
    std::shared_ptr<PDF> PDFPtr;
    bool                 SkipPDF;
    Ray                  SkipPDFRay;
};