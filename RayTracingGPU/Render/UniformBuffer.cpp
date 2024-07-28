#include "UniformBuffer.h"

#include <Engine/Buffer.h>

#include <cstring>

namespace Render
{

UniformBuffer::UniformBuffer(const Engine::Device& device)
{
    constexpr auto bufferSize = sizeof(UniformBufferObject);

    m_Buffer.reset(new Engine::Buffer(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
    m_Memory.reset(new Engine::DeviceMemory(m_Buffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)));
}

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept
    : m_Buffer(other.m_Buffer.release())
    , m_Memory(other.m_Memory.release())
{
}

UniformBuffer::~UniformBuffer()
{
    m_Buffer.reset();
    m_Memory.reset();    // release memory after bound buffer has been destroyed
}

void UniformBuffer::SetValue(const UniformBufferObject& ubo) const
{
    const auto data = m_Memory->Map(0, sizeof(UniformBufferObject));
    std::memcpy(data, &ubo, sizeof(ubo));
    m_Memory->Unmap();
}

}    // namespace Render
