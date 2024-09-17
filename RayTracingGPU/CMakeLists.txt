cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME ray-tracing-gpu)
project(${PROJECT_NAME} VERSION ${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}.${APP_VERSION_TWEAK} LANGUAGES CXX)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Resources/ray-tracing.rc.in" "${CMAKE_CURRENT_BINARY_DIR}/ray-tracing.rc")

set(SHADER_FILES
    Resources/Shaders/Common.glsl
    Resources/Shaders/Graphics.frag
    Resources/Shaders/Graphics.vert
    Resources/Shaders/Heatmap.glsl
    Resources/Shaders/Material.glsl
    Resources/Shaders/ProceduralHit.glsl
    Resources/Shaders/Random.glsl
    Resources/Shaders/RayPayload.glsl
    Resources/Shaders/RayTracing.Procedural.rchit
    Resources/Shaders/RayTracing.Procedural.rint
    Resources/Shaders/RayTracing.rchit
    Resources/Shaders/RayTracing.rgen
    Resources/Shaders/RayTracing.rmiss
    Resources/Shaders/Scatter.glsl
    Resources/Shaders/UniformBufferObject.glsl
    Resources/Shaders/Vertex.glsl
)

set(SOURCE_FILES
    Main.cpp
    # ${CMAKE_CURRENT_BINARY_DIR}/ray-tracing.rc
    ModelViewController.cpp
    ModelViewController.h
    RayTracer.cpp
    RayTracer.h

    Engine/Application.cpp
    Engine/Application.h
    Engine/Buffer.cpp
    Engine/Buffer.h
    Engine/BufferUtil.h
    Engine/CommandBuffers.cpp
    Engine/CommandBuffers.h
    Engine/CommandPool.cpp
    Engine/CommandPool.h
    Engine/DebugUtils.cpp
    Engine/DebugUtils.h
    Engine/DebugUtilsMessenger.cpp
    Engine/DebugUtilsMessenger.h
    Engine/DepthBuffer.cpp
    Engine/DepthBuffer.h
    Engine/DescriptorBinding.h
    Engine/DescriptorPool.cpp
    Engine/DescriptorPool.h
    Engine/DescriptorSetLayout.cpp
    Engine/DescriptorSetLayout.h
    Engine/DescriptorSetManager.cpp
    Engine/DescriptorSetManager.h
    Engine/DescriptorSets.cpp
    Engine/DescriptorSets.h
    Engine/Device.cpp
    Engine/Device.h
    Engine/DeviceMemory.cpp
    Engine/DeviceMemory.h
    Engine/Enumerate.h
    Engine/Fence.cpp
    Engine/Fence.h
    Engine/FrameBuffer.cpp
    Engine/FrameBuffer.h
    Engine/GraphicsPipeline.cpp
    Engine/GraphicsPipeline.h
    Engine/Image.cpp
    Engine/Image.h
    Engine/ImageMemoryBarrier.h
    Engine/ImageView.cpp
    Engine/ImageView.h
    Engine/Instance.cpp
    Engine/Instance.h
    Engine/PipelineLayout.cpp
    Engine/PipelineLayout.h
    Engine/RenderPass.cpp
    Engine/RenderPass.h
    Engine/Sampler.cpp
    Engine/Sampler.h
    Engine/Semaphore.cpp
    Engine/Semaphore.h
    Engine/ShaderModule.cpp
    Engine/ShaderModule.h
    Engine/SingleTimeCommands.h
    Engine/Strings.cpp
    Engine/Strings.h
    Engine/Surface.cpp
    Engine/Surface.h
    Engine/SwapChain.cpp
    Engine/SwapChain.h
    Engine/Version.h
    Engine/Window.cpp
    Engine/Window.h
    Engine/WindowConfig.h

    Objects/CornellBox.cpp
    Objects/CornellBox.h
    Objects/Model.cpp
    Objects/Model.h
    Objects/Procedural.h
    Objects/Sphere.h
    Objects/Vertex.h

    RayTracing/AccelerationStructure.cpp
    RayTracing/AccelerationStructure.h
    RayTracing/Application.cpp
    RayTracing/Application.h
    RayTracing/BottomLevelAccelerationStructure.cpp
    RayTracing/BottomLevelAccelerationStructure.h
    RayTracing/BottomLevelGeometry.cpp
    RayTracing/BottomLevelGeometry.h
    RayTracing/DeviceProcedures.cpp
    RayTracing/DeviceProcedures.h
    RayTracing/RayTracingPipeline.cpp
    RayTracing/RayTracingPipeline.h
    RayTracing/RayTracingProperties.cpp
    RayTracing/RayTracingProperties.h
    RayTracing/ShaderBindingTable.cpp
    RayTracing/ShaderBindingTable.h
    RayTracing/TopLevelAccelerationStructure.cpp
    RayTracing/TopLevelAccelerationStructure.h

    Render/Material.h
    Render/Texture.cpp
    Render/Texture.h
    Render/TextureImage.cpp
    Render/TextureImage.h
    Render/UniformBuffer.cpp
    Render/UniformBuffer.h

    Scenes/Scene.cpp
    Scenes/Scene.h
    Scenes/SceneList.cpp
    Scenes/SceneList.h

    UI/ImGuiHelper.cpp
    UI/ImGuiHelper.h
    UI/UserInterface.cpp
    UI/UserInterface.h
    UI/UserSettings.h

    Utils/Filesystem.cpp
    Utils/Filesystem.h
    Utils/Glm.h
    Utils/Log.cpp
    Utils/Log.h
    Utils/Random.cpp
    Utils/Random.h
    Utils/StbImage.cpp
    Utils/StbImage.h
    Utils/StringTools.cpp
    Utils/StringTools.h
    Utils/Timers.cpp
    Utils/Timers.h
    Utils/VulkanException.cpp
    Utils/VulkanException.h

    # Shaders
    ${SHADER_FILES}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Find glslc executable
find_program(GLSLC_EXECUTABLE NAMES glslc HINTS ENV VULKAN_SDK PATH_SUFFIXES bin)

if(NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found. Please make sure Vulkan SDK is installed and available in your system's PATH.")
endif()

find_program(Vulkan_GLSLANG_VALIDATOR NAMES glslangValidator HINTS ENV VULKAN_SDK PATH_SUFFIXES bin)

if(NOT Vulkan_GLSLANG_VALIDATOR)
    message(FATAL_ERROR "glslangValidator not found. Please make sure Vulkan SDK is installed and available in your system's PATH.")
endif()

# Copy resources to the build directory.
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(isMultiConfig) # In Miti-Config mode, we need to copy to the correct configuration directory.
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/DummyResources")
        message(FATAL_ERROR "\"${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/DummyResources\" exists, remove it!")
    endif()

    add_custom_target(
        always_run_target_gpu ALL
        DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/DummyResources" # fake! ensure we run!
    )

    add_custom_command(
        OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/DummyResources" # fake! ensure we run!
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Resources"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Resources"
    )

    foreach(SHADER_FILE ${SHADER_FILES})
        # Dummy target to ensure that resources are up-to-date
        if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Dummy${SHADER_FILE_NAME}")
            message(FATAL_ERROR "\"${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Dummy${SHADER_FILE_NAME}\" exists, remove it!")
        endif()

        set_source_files_properties(${SHADER_FILE} PROPERTIES HEADER_FILE_ONLY TRUE)
        get_filename_component(SHADER_FILE_NAME ${SHADER_FILE} NAME)
        get_filename_component(SHADER_FILE_EXT ${SHADER_FILE} EXT)

        if(NOT SHADER_FILE_EXT STREQUAL ".glsl")
            set(SHADER_OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Resources/Shaders/${SHADER_FILE_NAME}.spv")
            add_custom_command(
                OUTPUT
                "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Dummy${SHADER_FILE_NAME}" # fake! ensure we run!
                "${SHADER_OUTPUT_FILE}"

                # COMMAND ${GLSLC_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}" -o "${SHADER_OUTPUT_FILE}"
                COMMAND ${Vulkan_GLSLANG_VALIDATOR} --target-env vulkan1.2 -V "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}" -o "${SHADER_OUTPUT_FILE}"
                DEPENDS
                "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}"
            )
            list(APPEND SHADER_OUTPUT_FILES "${SHADER_OUTPUT_FILE}")
        endif()
    endforeach()

    # Dummy target to ensure that resources are up-to-date
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/DummyShaders")
        message(FATAL_ERROR "\"${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/DummyShaders\" exists, remove it!")
    endif()

    add_custom_target(
        always_run_shaders_target_gpu ALL
        DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/DummyShaders" # fake! ensure we run!
        ${SHADER_OUTPUT_FILES}
    )
else() # In single-config mode, we just copy to the build directory.
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/DummyResources")
        message(FATAL_ERROR "\"${CMAKE_CURRENT_BINARY_DIR}/DummyResources\" exists, remove it!")
    endif()

    add_custom_target(
        always_run_target_gpu ALL
        DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/DummyResources" # fake! ensure we run!
    )

    add_custom_command(
        OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/DummyResources" # fake! ensure we run!
        "${CMAKE_CURRENT_BINARY_DIR}/Resources"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
        "${CMAKE_CURRENT_BINARY_DIR}/Resources"
    )

    foreach(SHADER_FILE ${SHADER_FILES})
        # Dummy target to ensure that resources are up-to-date
        if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/Dummy${SHADER_FILE_NAME}")
            message(FATAL_ERROR "\"${CMAKE_CURRENT_BINARY_DIR}/Dummy${SHADER_FILE_NAME}\" exists, remove it!")
        endif()

        set_source_files_properties(${SHADER_FILE} PROPERTIES HEADER_FILE_ONLY TRUE)
        get_filename_component(SHADER_FILE_NAME ${SHADER_FILE} NAME)
        get_filename_component(SHADER_FILE_EXT ${SHADER_FILE} EXT)

        if(NOT SHADER_FILE_EXT STREQUAL ".glsl")
            set(SHADER_OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/Resources/Shaders/${SHADER_FILE_NAME}.spv")
            add_custom_command(
                OUTPUT
                "${CMAKE_CURRENT_BINARY_DIR}/Dummy${SHADER_FILE_NAME}" # fake! ensure we run!
                "${SHADER_OUTPUT_FILE}"

                # COMMAND ${GLSLC_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}" -o "${SHADER_OUTPUT_FILE}"
                COMMAND ${Vulkan_GLSLANG_VALIDATOR} --target-env vulkan1.2 -V "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}" -o "${SHADER_OUTPUT_FILE}"
                DEPENDS
                "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}"
            )
            list(APPEND SHADER_OUTPUT_FILES "${SHADER_OUTPUT_FILE}")
        endif()
    endforeach()

    # Dummy target to ensure that resources are up-to-date
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/DummyShaders")
        message(FATAL_ERROR "\"${CMAKE_CURRENT_BINARY_DIR}/DummyShaders\" exists, remove it!")
    endif()

    add_custom_target(
        always_run_shaders_target_gpu ALL
        DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/DummyShaders" # fake! ensure we run!
        ${SHADER_OUTPUT_FILES}
    )
endif()

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog REQUIRED)
find_package(Stb REQUIRED)
find_package(tinyobjloader REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME} ${EXE_PLATFORM_KEYWORD} ${SOURCE_FILES})
add_dependencies(${PROJECT_NAME} always_run_target_gpu)

# Enable PDB files in the Release configuration and a couple of useful flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC "/Zc:__cplusplus")
    target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:Release>:/Zi>")
    target_link_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/DEBUG:FASTLINK>")
    target_link_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:REF>")
    target_link_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>")
    target_compile_options(${PROJECT_NAME} PUBLIC "/MP")
endif()

target_link_libraries(
    ${PROJECT_NAME}
    glfw
    glm::glm
    imgui::imgui
    spdlog::spdlog
    tinyobjloader::tinyobjloader
    Vulkan::Vulkan
)

target_include_directories(${PROJECT_NAME} PRIVATE "." ${Stb_INCLUDE_DIR})
