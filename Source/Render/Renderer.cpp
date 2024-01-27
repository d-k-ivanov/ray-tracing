#include "Renderer.h"

#include <Render/Color.h>
#include <Utils/Random.h>

#include <execution>

void Renderer::OnResize(uint32_t width, uint32_t height)
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
            auto pixelColor = Color3(static_cast<double>(x) / (m_Image->GetWidth() - 1), static_cast<double>(y) / (m_Image->GetHeight() - 1), 0);

            m_ImageData[y * m_Image->GetWidth() + x] = GetColorRGBA(pixelColor, 1);
        }
    }
    // std::clog << "\rDone.                 \n";

    m_Image->SetData(m_ImageData);
}

void Renderer::RenderSingleCore(Camera& camera, const Hittable& world) const
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
                Ray r = camera.GetRay(static_cast<int>(x), static_cast<int>(y));
                pixelColor += camera.RayColor(r, camera.MaxDepth, world);
            }
            m_ImageData[y * m_Image->GetWidth() + x] = GetColorRGBA(pixelColor, camera.SamplesPerPixel);
        }
    }
    std::clog << "\rDone.                 \n";

    m_Image->SetData(m_ImageData);
}

void Renderer::RenderMultiCore(Camera& camera, const Hittable& world) const
{
    camera.Initialize();
    std::for_each(std::execution::par, m_ImageHeightIterator.begin(), m_ImageHeightIterator.end(), [this, &camera, &world](uint32_t y)
                  {
        std::clog << "\rScanlines remaining: " << (m_Image->GetHeight() - y) << ' ' << std::flush;
    	std::for_each(std::execution::par, m_ImageWidthIterator.begin(), m_ImageWidthIterator.end(),
    		[this, y, &camera, &world](uint32_t x)
    		{
    			Color3 pixelColor(0, 0, 0);
                for(int sample = 0; sample < camera.SamplesPerPixel; ++sample)
                {
                    Ray r = camera.GetRay(static_cast<int>(x), static_cast<int>(y));
                    pixelColor += camera.RayColor(r, camera.MaxDepth, world);
                }
                m_ImageData[y * m_Image->GetWidth() + x] = GetColorRGBA(pixelColor, camera.SamplesPerPixel);
    		}); });
    std::clog << "\rDone.                 \n";

    m_Image->SetData(m_ImageData);
}
