#include "Renderer.h"

#include "Camera.h"

#include <Objects/Hittable.h>
#include <Objects/HittableList.h>

#include <cstring>
#include <execution>

void Renderer::ResetPixelColorsAccumulator() const
{
    std::memset(m_PixelColorsAccum, 0, static_cast<uint64_t>(m_Image->GetWidth()) * m_Image->GetHeight() * sizeof(Color3));
}

void Renderer::SetImageSize(uint32_t width, uint32_t height)
{
    if(m_Image)
    {
        // Return, if the image size isn't changed
        if(m_Image->GetWidth() == width && m_Image->GetHeight() == height)
            return;

        m_Image->Resize(width, height);
    }
    else
    {
        m_Image = std::make_shared<Image>(width, height, ImageFormat::RGBA);
    }

    delete[] m_ImageData;
    m_ImageData = new uint32_t[static_cast<uint64_t>(width * height)];

    delete[] m_PixelColorsAccum;
    m_PixelColorsAccum = new Color3[static_cast<uint64_t>(width * height)];

    m_ImageWidthIterator.resize(width);
    for(uint32_t i = 0; i < width; i++)
    {
        m_ImageWidthIterator[i] = i;
    }
    m_ImageHeightIterator.resize(height);
    for(uint32_t i = 0; i < height; i++)
    {
        m_ImageHeightIterator[i] = i;
    }
}

void Renderer::RenderHelloWorld() const
{
    for(uint32_t y = 0; y < m_Image->GetHeight(); y++)
    {
        for(uint32_t x = 0; x < m_Image->GetWidth(); x++)
        {
            auto pixelColor = Color3(static_cast<double>(x) / (m_Image->GetWidth() - 1), static_cast<double>(y) / (m_Image->GetHeight() - 1), 0);

            m_ImageData[y * m_Image->GetWidth() + x] = GetColorRGBA(pixelColor, 1);
        }
    }
    m_Image->SetData(m_ImageData);
}

void Renderer::RenderRandom() const
{
    for(uint32_t y = 0; y < m_Image->GetHeight(); y++)
    {
        for(uint32_t x = 0; x < m_Image->GetWidth(); x++)
        {
            m_ImageData[y * m_Image->GetWidth() + x] = GetRandomColorRGBA();
        }
    }
    m_Image->SetData(m_ImageData);
}

void Renderer::Render(Camera& camera, const Hittable& world, const HittableList& lights)
{
    if(camera.IsAccumulating)
    {
        camera.SamplesPerPixel = m_FrameCounter;
    }
    camera.Initialize();

    switch(camera.RenderingType)
    {
        case Camera::RenderType::CPUOneCore:
            CPUOneCore(camera, world, lights);
            break;
        case Camera::RenderType::CPUMultiCore:
            CPUMultiCore(camera, world, lights);
            break;
        case Camera::RenderType::GPU:
            // TBA
            break;
    }

    std::clog << "\rDone.                 \n";
    m_Image->SetData(m_ImageData);

    if(camera.IsAccumulating)
    {
        m_FrameCounter++;
    }
}

void Renderer::CPUOneCore(const Camera& camera, const Hittable& world, const HittableList& lights) const
{
    if(camera.IsAccumulating && m_FrameCounter == 1)
    {
        ResetPixelColorsAccumulator();
    }

    for(uint32_t y = 0; y < m_Image->GetHeight(); y++)
    {
        std::clog << "\rScanlines remaining: " << (m_Image->GetHeight() - y) << ' ' << std::flush;
        for(uint32_t x = 0; x < m_Image->GetWidth(); x++)
        {
            if(camera.IsAccumulating)
            {
                Ray r = camera.GetRay(static_cast<int>(x), static_cast<int>(y));
                m_PixelColorsAccum[y * m_Image->GetWidth() + x] += camera.RayColor(r, camera.MaxDepth, world, lights);
                const Color3 pixelColor = m_PixelColorsAccum[y * m_Image->GetWidth() + x];

                m_ImageData[y * m_Image->GetWidth() + x] = GetColorRGBA(pixelColor, 1.0 / m_FrameCounter);
            }
            else
            {
                m_ImageData[y * m_Image->GetWidth() + x] = camera.GetPixel(x, y, world, lights);
            }
        }
    }
}

void Renderer::CPUMultiCore(Camera& camera, const Hittable& world, const HittableList& lights)
{
    if(camera.IsAccumulating && m_FrameCounter == 1)
    {
        ResetPixelColorsAccumulator();
    }

    std::for_each(
        std::execution::par, m_ImageHeightIterator.begin(), m_ImageHeightIterator.end(),
        [this, &camera, &world, &lights](uint32_t y)
        {
            std::clog << "\rScanlines remaining: " << (m_Image->GetHeight() - y) << ' ' << std::flush;
            std::for_each(
                std::execution::par, m_ImageWidthIterator.begin(), m_ImageWidthIterator.end(),
                [this, y, &camera, &world, &lights](const uint32_t x)
                {
                    if(camera.IsAccumulating)
                    {
                        Ray r = camera.GetRay(static_cast<int>(x), static_cast<int>(y));
                        m_PixelColorsAccum[y * m_Image->GetWidth() + x] += camera.RayColor(r, camera.MaxDepth, world, lights);
                        const Color3 pixelColor = m_PixelColorsAccum[y * m_Image->GetWidth() + x];

                        m_ImageData[y * m_Image->GetWidth() + x] = GetColorRGBA(pixelColor, 1.0 / m_FrameCounter);
                    }
                    else
                    {
                        m_ImageData[y * m_Image->GetWidth() + x] = camera.GetPixel(x, y, world, lights);
                    }
                });
        });
}
