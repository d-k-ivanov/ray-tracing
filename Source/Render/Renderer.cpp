#include "Renderer.h"

#include <Utils/Random.h>

#include "Color.h"

void Renderer::OnResize(uint32_t width, uint32_t height)
{
    if(m_Image)
    {
        // No resize necessary
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
}

void Renderer::Render(Camera& camera, const Hittable& world) const
{
    camera.Initialize();
    for(uint32_t y = 0; y < m_Image->GetHeight(); y++)
    {
        std::clog << "\rScanlines remaining: " << (m_Image->GetHeight() - y) << ' ' << std::flush;
        for(uint32_t x = 0; x < m_Image->GetWidth(); x++)
        {
            Color3 pixelColor(0, 0, 0);
            for(int sample = 0; sample < camera.SamplesPerPixel; ++sample)
            {
                Ray r = camera.GetRay(x, y);
                pixelColor += camera.RayColor(r, camera.MaxDepth, world);
            }
            m_ImageData[y * m_Image->GetWidth() + x] = GetColorRGBA(pixelColor, camera.SamplesPerPixel);
        }
    }
    std::clog << "\rDone.                 \n";

    m_Image->SetData(m_ImageData);
}

void Renderer::RenderRandom() const
{
    for(uint32_t y = 0; y < m_Image->GetHeight(); y++)
    {
        // std::clog << "\rScanlines remaining: " << (height - j) << ' ' << std::flush;
        for(uint32_t x = 0; x < m_Image->GetWidth(); x++)
        {
            m_ImageData[y * m_Image->GetWidth() + x] = GetRandomColorRGBA();
        }
    }
    // std::clog << "\rDone.                 \n";

    m_Image->SetData(m_ImageData);
}

void Renderer::RenderHelloWorld() const
{
    for(uint32_t y = 0; y < m_Image->GetHeight(); y++)
    {
        // std::clog << "\rScanlines remaining: " << (height - j) << ' ' << std::flush;
        for(uint32_t x = 0; x < m_Image->GetWidth(); x++)
        {
            auto pixelColor = Color3(static_cast<double>(x) / (m_Image->GetWidth() - 1),
                                     static_cast<double>(y) / (m_Image->GetHeight() - 1),
                                     0);

            m_ImageData[y * m_Image->GetWidth() + x] = GetColorRGBA(pixelColor, 1);
        }
    }
    // std::clog << "\rDone.                 \n";

    m_Image->SetData(m_ImageData);
}
