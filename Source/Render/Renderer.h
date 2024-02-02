#pragma once

#include "Camera.h"

#include <Objects/Hittable.h>
#include <Objects/HittableList.h>
#include <UI/Image.h>

#include <memory>
#include <vector>

class Renderer
{
public:
    Renderer() = default;

    uint32_t GetFrameCounter() const { return m_FrameCounter; }
    void     ResetFrameCounter() { m_FrameCounter = 1; }
    bool     IsAccumulating() const { return m_IsAccumulating; }

    void ResetPixelColorsAccumulator() const;

    void SetImageSize(uint32_t width, uint32_t height);
    void RenderRandom() const;
    void RenderHelloWorld() const;
    void CPUOneCore(Camera& camera, const Hittable& world, const HittableList& lights);
    void CPUOneCoreAccumulating(Camera& camera, const Hittable& world, const HittableList& lights);
    void CPUOneCoreStratified(Camera& camera, const Hittable& world, const HittableList& lights);
    void CPUMultiCore(Camera& camera, const Hittable& world, const HittableList& lights);
    void CPUMultiCoreAccumulating(Camera& camera, const Hittable& world, const HittableList& lights);
    void CPUMultiCoreStratified(Camera& camera, const Hittable& world, const HittableList& lights);
    void CPUMultiCoreStratifiedAccumulating(Camera& camera, const Hittable& world, const HittableList& lights);

    std::shared_ptr<Image> GetImage() const { return m_Image; }

private:
    int                    m_FrameCounter     = 1;
    int                    m_Xs               = 0;
    int                    m_Ys               = 0;
    bool                   m_IsAccumulating   = false;
    uint32_t*              m_ImageData        = nullptr;
    Color3*                m_PixelColorsAccum = nullptr;
    std::shared_ptr<Image> m_Image;
    std::vector<uint32_t>  m_ImageHeightIterator;
    std::vector<uint32_t>  m_ImageWidthIterator;
};