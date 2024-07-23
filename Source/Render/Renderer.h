#pragma once

#include "Color.h"

#include <Vulkan/Image.h>

#include <memory>
#include <vector>

namespace Objects
{
class Hittable;
class HittableList;
}    // namespace Render

namespace Render
{

class Camera;

class Renderer
{
public:
    Renderer() = default;

    uint32_t GetFrameCounter() const { return m_FrameCounter; }
    void     ResetFrameCounter() { m_FrameCounter = 1; }

    void ResetPixelColorsAccumulator() const;

    void SetImageSize(uint32_t width, uint32_t height);
    void RenderRandom() const;
    void RenderHelloWorld() const;
    void Render(Camera& camera, const Objects::Hittable& world, const Objects::HittableList& lights);
    void CPUOneCore(const Camera& camera, const Objects::Hittable& world, const Objects::HittableList& lights) const;
    void CPUMultiCore(Camera& camera, const Objects::Hittable& world, const Objects::HittableList& lights);

    std::shared_ptr<Vulkan::Image> GetImage() const { return m_Image; }

private:
    int                            m_FrameCounter     = 1;
    int                            m_Xs               = 0;
    int                            m_Ys               = 0;
    bool                           m_IsAccumulating   = false;
    uint32_t*                      m_ImageData        = nullptr;
    Color3*                        m_PixelColorsAccum = nullptr;
    std::shared_ptr<Vulkan::Image> m_Image;
    std::vector<uint32_t>          m_ImageHeightIterator;
    std::vector<uint32_t>          m_ImageWidthIterator;
};

}    // namespace Render
