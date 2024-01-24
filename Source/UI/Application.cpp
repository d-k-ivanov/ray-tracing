// Derived from https://github.com/ocornut/imgui/blob/docking/examples/example_glfw_vulkan/main.cpp
#include "Application.h"

#include <Utils/Filesystem.h>
#include <Utils/Log.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "Utils/VulkanException.h"

extern bool g_ApplicationRunning;

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// #define IMGUI_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif
namespace Impl
{

VkAllocationCallbacks*   g_Allocator      = nullptr;
VkInstance               g_Instance       = VK_NULL_HANDLE;
VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
VkDevice                 g_Device         = VK_NULL_HANDLE;
uint32_t                 g_QueueFamily    = static_cast<uint32_t>(-1);
VkQueue                  g_Queue          = VK_NULL_HANDLE;
VkDebugReportCallbackEXT g_DebugReport    = VK_NULL_HANDLE;
VkPipelineCache          g_PipelineCache  = VK_NULL_HANDLE;
VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

ImGui_ImplVulkanH_Window g_MainWindowData;
uint32_t                 g_MinImageCount    = 2;
bool                     g_SwapChainRebuild = false;

// Per-frame-in-flight
std::vector<std::vector<VkCommandBuffer>>       g_AllocatedCommandBuffers;
std::vector<std::vector<std::function<void()>>> g_ResourceFreeQueue;

// Unlike g_MainWindowData.FrameIndex, this is not the the swapchain image index
// and is always guaranteed to increase (eg. 0, 1, 2, 0, 1, 2)
uint32_t g_CurrentFrameIndex = 0;

Application* g_AppInstance = nullptr;

#ifdef IMGUI_VULKAN_DEBUG_REPORT
VKAPI_ATTR VkBool32 VKAPI_CALL DebugReport(const VkDebugReportFlagsEXT      flags,
                                           const VkDebugReportObjectTypeEXT objectType,
                                           const uint64_t                   object,
                                           const size_t                     location,
                                           const int32_t                    messageCode,
                                           const char*                      pLayerPrefix,
                                           const char*                      pMessage,
                                           void*                            pUserData)
{
    (void)flags;
    (void)object;
    (void)location;
    (void)messageCode;
    (void)pUserData;
    (void)pLayerPrefix;
    LOG_WARN(ConcatenateStrings("[vulkan] Debug report from ObjectType:", objectType, " Message: ", pMessage));
    // (void)fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
    return VK_FALSE;
}
#endif    // IMGUI_VULKAN_DEBUG_REPORT

bool IsExtensionAvailable(const ImVector<VkExtensionProperties>& properties, const char* extension)
{
    for(const VkExtensionProperties& p : properties)
        if(strcmp(p.extensionName, extension) == 0)
            return true;
    return false;
}

VkPhysicalDevice selectPhysicalDevice()
{
    uint32_t gpuCount;
    VK_CHECK(vkEnumeratePhysicalDevices(g_Instance, &gpuCount, nullptr));
    IM_ASSERT(gpuCount > 0);

    ImVector<VkPhysicalDevice> gpus;
    gpus.resize(static_cast<int>(gpuCount));
    VK_CHECK(vkEnumeratePhysicalDevices(g_Instance, &gpuCount, gpus.Data));

    // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
    // most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
    // dedicated GPUs) is out of scope of this sample.
    for(const VkPhysicalDevice& device : gpus)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);
        if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            return device;
    }

    // Use first GPU (Integrated) is a Discrete one is not available.
    if(gpuCount > 0)
        return gpus[0];
    return VK_NULL_HANDLE;
}

void SetupVulkan(ImVector<const char*> instanceExtensions)
{
    // Create Vulkan Instance
    {
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        // Enumerate available extensions
        uint32_t                        propertiesCount;
        ImVector<VkExtensionProperties> properties;
        vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr);
        properties.resize(static_cast<int>(propertiesCount));
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, properties.Data));

        // Enable required extensions
        if(IsExtensionAvailable(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
            instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
        if(IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
        {
            instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }
#endif

        // Enabling validation layers
#ifdef IMGUI_VULKAN_DEBUG_REPORT
        const char* layers[]           = {"VK_LAYER_KHRONOS_validation"};
        createInfo.enabledLayerCount   = 1;
        createInfo.ppEnabledLayerNames = layers;
        instanceExtensions.push_back("VK_EXT_debug_report");
#endif

        // Create Vulkan Instance
        createInfo.enabledExtensionCount   = static_cast<uint32_t>(instanceExtensions.Size);
        createInfo.ppEnabledExtensionNames = instanceExtensions.Data;
        VK_CHECK(vkCreateInstance(&createInfo, g_Allocator, &g_Instance));

        // Setup the debug report callback
#ifdef IMGUI_VULKAN_DEBUG_REPORT
        const auto vkCreateDebugReportCallbackExt = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(g_Instance, "vkCreateDebugReportCallbackEXT"));
        IM_ASSERT(vkCreateDebugReportCallbackExt != nullptr);
        VkDebugReportCallbackCreateInfoEXT debugReportCi = {};
        debugReportCi.sType                              = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        debugReportCi.flags                              = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debugReportCi.pfnCallback                        = DebugReport;
        debugReportCi.pUserData                          = nullptr;
        VK_CHECK(vkCreateDebugReportCallbackExt(g_Instance, &debugReportCi, g_Allocator, &g_DebugReport));
#endif
    }

    // Select Physical Device (GPU)
    g_PhysicalDevice = selectPhysicalDevice();

    // Select graphics queue family
    {
        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, nullptr);
        VkQueueFamilyProperties* queues = static_cast<VkQueueFamilyProperties*>(malloc(sizeof(VkQueueFamilyProperties) * count));
        vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queues);
        for(uint32_t i = 0; i < count; i++)
            if(queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                g_QueueFamily = i;
                break;
            }
        free(queues);
        IM_ASSERT(g_QueueFamily != static_cast<uint32_t>(-1));
    }

    // Create Logical Device (with 1 queue)
    {
        ImVector<const char*> device_extensions;
        device_extensions.push_back("VK_KHR_swapchain");

        // Enumerate physical device extension
        uint32_t                        propertiesCount;
        ImVector<VkExtensionProperties> properties;
        vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &propertiesCount, nullptr);
        properties.resize(static_cast<int>(propertiesCount));
        vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &propertiesCount, properties.Data);
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
        if(IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
            device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

        constexpr float         queuePriority[] = {1.0f};
        VkDeviceQueueCreateInfo queueInfo[1]    = {};
        queueInfo[0].sType                      = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo[0].queueFamilyIndex           = g_QueueFamily;
        queueInfo[0].queueCount                 = 1;
        queueInfo[0].pQueuePriorities           = queuePriority;
        VkDeviceCreateInfo createInfo           = {};
        createInfo.sType                        = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount         = std::size(queueInfo);
        createInfo.pQueueCreateInfos            = queueInfo;
        createInfo.enabledExtensionCount        = static_cast<uint32_t>(device_extensions.Size);
        createInfo.ppEnabledExtensionNames      = device_extensions.Data;
        VK_CHECK(vkCreateDevice(g_PhysicalDevice, &createInfo, g_Allocator, &g_Device));
        vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
    }

    // Create Descriptor Pool
    // The example only requires a single combined image sampler descriptor for the font image and only uses one descriptor set (for that)
    // If you wish to load e.g. additional textures you may need to alter pools sizes.
    {
        const VkDescriptorPoolSize poolSizes[] = {
            {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
            // { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets                    = 1000 * IM_ARRAYSIZE(poolSizes);
        // poolInfo.maxSets = 1;
        poolInfo.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(poolSizes));
        poolInfo.pPoolSizes    = poolSizes;
        VK_CHECK(vkCreateDescriptorPool(g_Device, &poolInfo, g_Allocator, &g_DescriptorPool));
    }
}

// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// Your real engine/app may not use them.
void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, const VkSurfaceKHR surface, const int width, const int height)
{
    wd->Surface = surface;

    // Check for WSI support
    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, wd->Surface, &res);
    if(res != VK_TRUE)
    {
        (void)fprintf(stderr, "Error no WSI support on physical device 0\n");
        exit(-1);
    }

    // Select Surface Format
    constexpr VkFormat        requestSurfaceImageFormat[] = {VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
    constexpr VkColorSpaceKHR requestSurfaceColorSpace    = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

    wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

    // Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
    VkPresentModeKHR presentModes[] = {VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR};
#else
    VkPresentModeKHR presentModes[] = {VK_PRESENT_MODE_FIFO_KHR};
#endif
    wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, wd->Surface, &presentModes[0], IM_ARRAYSIZE(presentModes));
    // printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

    // Create SwapChain, RenderPass, Framebuffer, etc.
    IM_ASSERT(g_MinImageCount >= 2);
    ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
}

void CleanupVulkan()
{
    vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);

#ifdef IMGUI_VULKAN_DEBUG_REPORT
    // Remove the debug report callback
    const auto vkDestroyDebugReportCallbackExt = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(g_Instance, "vkDestroyDebugReportCallbackEXT"));
    vkDestroyDebugReportCallbackExt(g_Instance, g_DebugReport, g_Allocator);
#endif    // IMGUI_VULKAN_DEBUG_REPORT

    vkDestroyDevice(g_Device, g_Allocator);
    vkDestroyInstance(g_Instance, g_Allocator);
}

void CleanupVulkanWindow()
{
    ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &g_MainWindowData, g_Allocator);
}

void FrameRender(ImGui_ImplVulkanH_Window* window, ImDrawData* drawData)
{
    const VkSemaphore imageAcquiredSemaphore  = window->FrameSemaphores[window->SemaphoreIndex].ImageAcquiredSemaphore;
    const VkSemaphore renderCompleteSemaphore = window->FrameSemaphores[window->SemaphoreIndex].RenderCompleteSemaphore;

    do
    {
        const VkResult result = vkAcquireNextImageKHR(g_Device, window->Swapchain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &window->FrameIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            g_SwapChainRebuild = true;
            return;
        }
        if(VK_SUCCESS != result)
        {
            LOG_ERROR(ConcatenateStrings("vkAcquireNextImageKHR", " returned ", result));
            throw VulkanException(
                {__func__, __FILE__, static_cast<uint32_t>(__LINE__)},
                ConcatenateStrings("vkAcquireNextImageKHR", " returned ", result));
        }
    } while(false);

    g_CurrentFrameIndex = (g_CurrentFrameIndex + 1) % g_MainWindowData.ImageCount;

    const ImGui_ImplVulkanH_Frame* frame = &window->Frames[window->FrameIndex];
    {
        VK_CHECK(vkWaitForFences(g_Device, 1, &frame->Fence, VK_TRUE, UINT64_MAX));
        VK_CHECK(vkResetFences(g_Device, 1, &frame->Fence));
    }
    {
        // Free resources in queue
        for(auto& func : g_ResourceFreeQueue[g_CurrentFrameIndex])
            func();
        g_ResourceFreeQueue[g_CurrentFrameIndex].clear();
    }
    {
        // Free command buffers allocated by Application::GetCommandBuffer
        // These use g_MainWindowData.FrameIndex and not s_CurrentFrameIndex because they're tied to the swapchain image index
        auto& allocatedCommandBuffers = g_AllocatedCommandBuffers[window->FrameIndex];
        if(!allocatedCommandBuffers.empty())
        {
            vkFreeCommandBuffers(g_Device, frame->CommandPool, static_cast<uint32_t>(allocatedCommandBuffers.size()), allocatedCommandBuffers.data());
            allocatedCommandBuffers.clear();
        }

        VK_CHECK(vkResetCommandPool(g_Device, frame->CommandPool, 0));
        VkCommandBufferBeginInfo info = {};
        info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_CHECK(vkBeginCommandBuffer(frame->CommandBuffer, &info));
    }
    {
        VkRenderPassBeginInfo info    = {};
        info.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass               = window->RenderPass;
        info.framebuffer              = frame->Framebuffer;
        info.renderArea.extent.width  = window->Width;
        info.renderArea.extent.height = window->Height;
        info.clearValueCount          = 1;
        info.pClearValues             = &window->ClearValue;
        vkCmdBeginRenderPass(frame->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData(drawData, frame->CommandBuffer);

    // Submit command buffer
    vkCmdEndRenderPass(frame->CommandBuffer);
    {
        constexpr VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo                   info      = {};
        info.sType                               = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.waitSemaphoreCount                  = 1;
        info.pWaitSemaphores                     = &imageAcquiredSemaphore;
        info.pWaitDstStageMask                   = &waitStage;
        info.commandBufferCount                  = 1;
        info.pCommandBuffers                     = &frame->CommandBuffer;
        info.signalSemaphoreCount                = 1;
        info.pSignalSemaphores                   = &renderCompleteSemaphore;

        VK_CHECK(vkEndCommandBuffer(frame->CommandBuffer));
        VK_CHECK(vkQueueSubmit(g_Queue, 1, &info, frame->Fence));
    }
}

void FramePresent(ImGui_ImplVulkanH_Window* window)
{
    if(g_SwapChainRebuild)
        return;
    const VkSemaphore renderCompleteSemaphore = window->FrameSemaphores[window->SemaphoreIndex].RenderCompleteSemaphore;
    VkPresentInfoKHR  info                    = {};
    info.sType                                = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount                   = 1;
    info.pWaitSemaphores                      = &renderCompleteSemaphore;
    info.swapchainCount                       = 1;
    info.pSwapchains                          = &window->Swapchain;
    info.pImageIndices                        = &window->FrameIndex;
    do
    {
        const VkResult result = vkQueuePresentKHR(g_Queue, &info);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            g_SwapChainRebuild = true;
            return;
        }
        if(VK_SUCCESS != result)
        {
            LOG_ERROR(ConcatenateStrings("vkQueuePresentKHR", " returned ", result));
            throw VulkanException(
                {__func__, __FILE__, static_cast<uint32_t>(__LINE__)},
                ConcatenateStrings("vkQueuePresentKHR", " returned ", result));
        }
    } while(false);
    window->SemaphoreIndex = (window->SemaphoreIndex + 1) % window->ImageCount;    // Now we can use the next set of semaphores
    VK_CHECK(vkDeviceWaitIdle(g_Device));
}

void GLFWErrorCallback(const int error, const char* description)
{
    (void)fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void VkCheckResultCallback(const VkResult result)
{
    if(result == 0)
        return;
    LOG_ERROR(ConcatenateStrings("[vulkan] Error: VkResult = ", result));
    if(result < 0)
        abort();
}

}    // namespace Impl

Application::Application(ApplicationSpecs specification)
    : m_Specification(std::move(specification))
{
    Impl::g_AppInstance = this;
    Init();
}

Application::~Application()
{
    Shutdown();
    Impl::g_AppInstance = nullptr;
}

Application& Application::Get()
{
    return *Impl::g_AppInstance;
}

void Application::Init()
{
    // Setup GLFW window
    glfwSetErrorCallback(Impl::GLFWErrorCallback);
    if(glfwInit() != GLFW_TRUE)
    {
        std::cerr << "Could not initalize GLFW!\n";
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_WindowHandle = glfwCreateWindow(m_Specification.Width, m_Specification.Height, m_Specification.Name.c_str(), nullptr, nullptr);

    // Setup Vulkan
    if(!glfwVulkanSupported())
    {
        std::cerr << "GLFW: Vulkan not supported!\n";
        return;
    }

    ImVector<const char*> extensions;
    uint32_t              extensionsCount = 0;
    const char**          glfwExtensions  = glfwGetRequiredInstanceExtensions(&extensionsCount);
    for(uint32_t i = 0; i < extensionsCount; i++)
        extensions.push_back(glfwExtensions[i]);
    Impl::SetupVulkan(extensions);

    // Create Window Surface
    VkSurfaceKHR surface;
    VK_CHECK(glfwCreateWindowSurface(Impl::g_Instance, m_WindowHandle, Impl::g_Allocator, &surface));

    // Create Framebuffers
    int w, h;
    glfwGetFramebufferSize(m_WindowHandle, &w, &h);
    ImGui_ImplVulkanH_Window* wd = &Impl::g_MainWindowData;
    Impl::SetupVulkanWindow(wd, surface, w, h);

    Impl::g_AllocatedCommandBuffers.resize(wd->ImageCount);
    Impl::g_ResourceFreeQueue.resize(wd->ImageCount);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;        // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;      // Enable Multi-Viewport / Platform Windows
    // io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; // Disable mouse cursos changing
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;    // DPI awareness
    // io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();
    // ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding              = 0.0f;
        style.WindowBorderSize            = 0.0f;
        style.FrameBorderSize             = 1.0f;
        style.FrameRounding               = 5.0f;
        style.WindowMenuButtonPosition    = ImGuiDir_None;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(m_WindowHandle, true);
    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance                  = Impl::g_Instance;
    initInfo.PhysicalDevice            = Impl::g_PhysicalDevice;
    initInfo.Device                    = Impl::g_Device;
    initInfo.QueueFamily               = Impl::g_QueueFamily;
    initInfo.Queue                     = Impl::g_Queue;
    initInfo.PipelineCache             = Impl::g_PipelineCache;
    initInfo.DescriptorPool            = Impl::g_DescriptorPool;
    initInfo.Subpass                   = 0;
    initInfo.MinImageCount             = Impl::g_MinImageCount;
    initInfo.ImageCount                = wd->ImageCount;
    initInfo.MSAASamples               = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator                 = Impl::g_Allocator;
    initInfo.CheckVkResultFn           = Impl::VkCheckResultCallback;
    ImGui_ImplVulkan_Init(&initInfo, wd->RenderPass);

    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;
    auto    robotoFontPath          = ThisExecutableLocation() + "/Resources/Fonts/Roboto-Regular.ttf";
    ImFont* robotoFont              = io.Fonts->AddFontFromFileTTF(robotoFontPath.c_str(), 14.0f);
    IM_ASSERT(robotoFont != nullptr);
    io.FontDefault = robotoFont;

    // Upload Fonts
    {
        // Use any command queue
        VkCommandPool   commandPool   = wd->Frames[wd->FrameIndex].CommandPool;
        VkCommandBuffer commandBuffer = wd->Frames[wd->FrameIndex].CommandBuffer;

        VK_CHECK(vkResetCommandPool(Impl::g_Device, commandPool, 0));
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

        // ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
        ImGui_ImplVulkan_CreateFontsTexture();

        VkSubmitInfo endInfo       = {};
        endInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        endInfo.commandBufferCount = 1;
        endInfo.pCommandBuffers    = &commandBuffer;
        VK_CHECK(vkEndCommandBuffer(commandBuffer));
        VK_CHECK(vkQueueSubmit(Impl::g_Queue, 1, &endInfo, VK_NULL_HANDLE));
        VK_CHECK(vkDeviceWaitIdle(Impl::g_Device));
    }
}

void Application::Shutdown()
{
    for(const auto& layer : m_LayerStack)
        layer->OnDetach();

    m_LayerStack.clear();

    // Cleanup
    VK_CHECK(vkDeviceWaitIdle(Impl::g_Device));

    // Free resources in queue
    for(auto& queue : Impl::g_ResourceFreeQueue)
    {
        for(auto& func : queue)
            func();
    }
    Impl::g_ResourceFreeQueue.clear();

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    Impl::CleanupVulkanWindow();
    Impl::CleanupVulkan();

    glfwDestroyWindow(m_WindowHandle);
    glfwTerminate();

    g_ApplicationRunning = false;
}

void Application::Run()
{
    m_Running = true;

    ImGui_ImplVulkanH_Window* window     = &Impl::g_MainWindowData;
    constexpr ImVec4          clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    const ImGuiIO&            io         = ImGui::GetIO();

    // Main loop
    while(glfwWindowShouldClose(m_WindowHandle) == GLFW_FALSE && m_Running)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        for(const auto& layer : m_LayerStack)
            layer->OnUpdate(m_TimeStep);

        // Resize swap chain?
        if(Impl::g_SwapChainRebuild)
        {
            int width, height;
            glfwGetFramebufferSize(m_WindowHandle, &width, &height);
            if(width > 0 && height > 0)
            {
                ImGui_ImplVulkan_SetMinImageCount(Impl::g_MinImageCount);
                ImGui_ImplVulkanH_CreateOrResizeWindow(Impl::g_Instance, Impl::g_PhysicalDevice, Impl::g_Device, &Impl::g_MainWindowData, Impl::g_QueueFamily, Impl::g_Allocator, width, height, Impl::g_MinImageCount);
                Impl::g_MainWindowData.FrameIndex = 0;

                // Clear allocated command buffers from here since entire pool is destroyed
                Impl::g_AllocatedCommandBuffers.clear();
                Impl::g_AllocatedCommandBuffers.resize(Impl::g_MainWindowData.ImageCount);

                Impl::g_SwapChainRebuild = false;
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
            if(m_MenubarCallback != nullptr)
                windowFlags |= ImGuiWindowFlags_MenuBar;

            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

            // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
            // and handle the pass-thru hole, so we ask Begin() to not render a background.
            if(dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
                windowFlags |= ImGuiWindowFlags_NoBackground;

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace", nullptr, windowFlags);
            ImGui::PopStyleVar();

            ImGui::PopStyleVar(2);

            // Submit the DockSpace
            // ImGuiIO& io = ImGui::GetIO();
            if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                const ImGuiID dockspaceId = ImGui::GetID("ApplicationDockspace");
                ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
            }

            if(m_MenubarCallback != nullptr)
            {
                if(ImGui::BeginMenuBar())
                {
                    m_MenubarCallback();
                    ImGui::EndMenuBar();
                }
            }

            for(const auto& layer : m_LayerStack)
                layer->OnUIRender();

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        ImDrawData* mainDrawData            = ImGui::GetDrawData();
        const bool  mainIsMinimized         = (mainDrawData->DisplaySize.x <= 0.0f || mainDrawData->DisplaySize.y <= 0.0f);
        window->ClearValue.color.float32[0] = clearColor.x * clearColor.w;
        window->ClearValue.color.float32[1] = clearColor.y * clearColor.w;
        window->ClearValue.color.float32[2] = clearColor.z * clearColor.w;
        window->ClearValue.color.float32[3] = clearColor.w;
        if(!mainIsMinimized)
            Impl::FrameRender(window, mainDrawData);

        // Update and Render additional Platform Windows
        if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        // Present Main Platform Window
        if(!mainIsMinimized)
            Impl::FramePresent(window);

        const double time = GetTime();
        m_FrameTime       = time - m_LastFrameTime;
        m_TimeStep        = std::min<double>(m_FrameTime, 0.0333);
        m_LastFrameTime   = time;
    }
}

void Application::Close()
{
    m_Running = false;
}

double Application::GetTime() const
{
    return glfwGetTime();
}

VkInstance Application::GetInstance()
{
    return Impl::g_Instance;
}

VkPhysicalDevice Application::GetPhysicalDevice()
{
    return Impl::g_PhysicalDevice;
}

VkDevice Application::GetDevice()
{
    return Impl::g_Device;
}

VkCommandBuffer Application::GetCommandBuffer(bool begin)
{
    const ImGui_ImplVulkanH_Window* window = &Impl::g_MainWindowData;

    // Use any command queue
    const VkCommandPool commandPool = window->Frames[window->FrameIndex].CommandPool;

    VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
    cmdBufAllocateInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocateInfo.commandPool                 = commandPool;
    cmdBufAllocateInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufAllocateInfo.commandBufferCount          = 1;

    VkCommandBuffer& commandBuffer = Impl::g_AllocatedCommandBuffers[window->FrameIndex].emplace_back();

    VK_CHECK(vkAllocateCommandBuffers(Impl::g_Device, &cmdBufAllocateInfo, &commandBuffer));

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    return commandBuffer;
}

void Application::FlushCommandBuffer(const VkCommandBuffer commandBuffer)
{
    constexpr uint64_t defaultFenceTimeout = 100000000000;

    VkSubmitInfo endInfo       = {};
    endInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    endInfo.commandBufferCount = 1;
    endInfo.pCommandBuffers    = &commandBuffer;
    VK_CHECK(vkEndCommandBuffer(commandBuffer));

    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags             = 0;
    VkFence fence;
    VK_CHECK(vkCreateFence(Impl::g_Device, &fenceCreateInfo, nullptr, &fence));
    VK_CHECK(vkQueueSubmit(Impl::g_Queue, 1, &endInfo, fence));
    VK_CHECK(vkWaitForFences(Impl::g_Device, 1, &fence, VK_TRUE, defaultFenceTimeout));
    vkDestroyFence(Impl::g_Device, fence, nullptr);
}

void Application::SubmitResourceFree(std::function<void()>&& func)
{
    Impl::g_ResourceFreeQueue[Impl::g_CurrentFrameIndex].emplace_back(std::move(func));
}
