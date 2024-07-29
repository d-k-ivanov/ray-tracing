#pragma once

#include "Procedural.h"
#include "Vertex.h"

#include <Render/Material.h>

#include <memory>
#include <string>
#include <vector>

namespace Objects
{
class Model final
{
public:
    Model()             = default;
    Model(const Model&) = default;
    Model(Model&&)      = default;
    ~Model()            = default;

    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&)      = delete;

    static Model LoadModel(const std::string& filename);
    static Model CreateCornellBox(float scale, bool light = true);
    static Model CreateBox(const glm::vec3& p0, const glm::vec3& p1, const Render::Material& material);
    static Model CreateSphere(const glm::vec3& center, float radius, const Render::Material& material, bool isProcedural);

    void SetMaterial(const Render::Material& material);
    void Transform(const glm::mat4& transform);

    const std::vector<Vertex>&           Vertices() const { return m_Vertices; }
    const std::vector<uint32_t>&         Indices() const { return m_Indices; }
    const std::vector<Render::Material>& Materials() const { return m_Materials; }

    const Procedural* Procedural() const { return m_Procedural.get(); }

    uint32_t NumberOfVertices() const { return static_cast<uint32_t>(m_Vertices.size()); }
    uint32_t NumberOfIndices() const { return static_cast<uint32_t>(m_Indices.size()); }
    uint32_t NumberOfMaterials() const { return static_cast<uint32_t>(m_Materials.size()); }

private:
    Model(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<Render::Material>&& materials, const class Procedural* procedural);

    std::vector<Vertex>                     m_Vertices;
    std::vector<uint32_t>                   m_Indices;
    std::vector<Render::Material>           m_Materials;
    std::shared_ptr<const class Procedural> m_Procedural;
};

}    // namespace Objects
