#pragma once

#include "Vertex.h"

#include <Render/Material.h>

#include <vector>

namespace Objects
{

class CornellBox final
{
public:
    static void Create(float scale, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, std::vector<Render::Material>& materials);
    static void CreateNoLight(float scale, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, std::vector<Render::Material>& materials);
};

}    // namespace Objects
