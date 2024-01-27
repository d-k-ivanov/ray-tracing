#pragma once

#include <Render/Camera.h>
#include <Render/Hittable.h>
#include <UI/Image.h>

#include <memory>
#include <vector>

class Renderer
{
public:
    Renderer() = default;

    void OnResize(uint32_t width, uint32_t height);
    void RenderRandom() const;
    void RenderHelloWorld() const;
    void RenderSingleCore(Camera& camera, const Hittable& world) const;
    void RenderMultiCore(Camera& camera, const Hittable& world) const;

    std::shared_ptr<Image> GetImage() const { return m_Image; }

private:
    uint32_t*              m_ImageData = nullptr;
    std::shared_ptr<Image> m_Image;
    std::vector<uint32_t>  m_ImageHeightIterator;
    std::vector<uint32_t>  m_ImageWidthIterator;
};