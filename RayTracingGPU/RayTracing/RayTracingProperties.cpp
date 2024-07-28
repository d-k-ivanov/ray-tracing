#include "RayTracingProperties.h"

#include <Engine/Device.h>

namespace RayTracing
{

RayTracingProperties::RayTracingProperties(const Engine::Device& device)
    : m_Device(device)
{
    m_AccelProps.sType    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
    m_PipelineProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
    m_PipelineProps.pNext = &m_AccelProps;

    VkPhysicalDeviceProperties2 props = {};
    props.sType                       = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    props.pNext                       = &m_PipelineProps;
    vkGetPhysicalDeviceProperties2(device.PhysicalDevice(), &props);
}

}    // namespace RayTracing
