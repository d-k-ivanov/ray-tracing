#pragma once

#include <Render/Camera.h>
#include <UI/Image.h>

#include <memory>

class Renderer
{
public:
    Renderer() = default;

    void OnResize(uint32_t width, uint32_t height);
    void Render(Camera& camera, const Hittable& world) const;
    void RenderRandom() const;
    void RenderHelloWorld() const;

    std::shared_ptr<Image> GetImage() const { return m_Image; }

private:
    uint32_t*              m_ImageData = nullptr;
    std::shared_ptr<Image> m_Image;
};