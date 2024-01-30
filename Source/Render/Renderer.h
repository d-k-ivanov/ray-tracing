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

    void ResetFrameCounter()
    {
        memset(m_PixelColorsAccum, 0, static_cast<uint64_t>(m_Image->GetWidth()) * m_Image->GetHeight() * sizeof(Color3));
        m_FrameCounter = 1;
    }

    uint32_t GetFrameCounter() const
    {
        return m_FrameCounter;
    }

    void SetSize(uint32_t width, uint32_t height);
    void RenderRandom() const;
    void RenderHelloWorld() const;
    void CPUOneCore(Camera& camera, const Hittable& world, const HittableList& lights) const;
    void CPUOneCoreAccumSamples(Camera& camera, const Hittable& world, const HittableList& lights);
    void CPUMultiCore(Camera& camera, const Hittable& world, const HittableList& lights) const;
    void CPUMultiCoreAccumSamples(Camera& camera, const Hittable& world, const HittableList& lights);
    void CPUMultiCoreStratified(Camera& camera, const Hittable& world, const HittableList& lights) const;

    std::shared_ptr<Image> GetImage() const { return m_Image; }

private:
    uint32_t               m_FrameCounter     = 1;
    uint32_t*              m_ImageData        = nullptr;
    Color3*                m_PixelColorsAccum = nullptr;
    std::shared_ptr<Image> m_Image;
    std::vector<uint32_t>  m_ImageHeightIterator;
    std::vector<uint32_t>  m_ImageWidthIterator;
};