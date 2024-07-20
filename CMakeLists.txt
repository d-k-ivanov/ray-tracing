cmake_minimum_required(VERSION 3.22)

option(VERBOSE_OUTPUT OFF)

set(PROJECT_NAME ray-tracing)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

include(InitVCPKG)
include(Functions)
include(ToolchainUpdate)
include(GitVersion)

# Git Version implementation for CMake
get_version_info("APP")

# Initialize Date and Time variables
init_time_and_date_variables()

project(${PROJECT_NAME} VERSION ${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}.${APP_VERSION_TWEAK} LANGUAGES CXX)

set(EXE_PLATFORM_KEYWORD "")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(EXE_PLATFORM_KEYWORD "")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(EXE_PLATFORM_KEYWORD "MACOSX_BUNDLE")
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Resources/ray-tracing.rc.in" "${CMAKE_CURRENT_BINARY_DIR}/ray-tracing.rc")

set(SHADER_FILES
    # Shaders will be here
)

set(SOURCE_FILES
    Source/Main.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/ray-tracing.rc

    Source/Math/AABB.cpp
    Source/Math/AABB.h
    Source/Math/Constants.cpp
    Source/Math/Constants.h
    Source/Math/Converters.cpp
    Source/Math/Converters.h
    Source/Math/CosinePDF.cpp
    Source/Math/CosinePDF.h
    Source/Math/HittablePDF.cpp
    Source/Math/HittablePDF.h
    Source/Math/Interval.cpp
    Source/Math/Interval.h
    Source/Math/MixturePDF.cpp
    Source/Math/MixturePDF.h
    Source/Math/ONB.cpp
    Source/Math/ONB.h
    Source/Math/PDF.cpp
    Source/Math/PDF.h
    Source/Math/SpherePDF.cpp
    Source/Math/SpherePDF.h
    Source/Math/Vector3.cpp
    Source/Math/Vector3.h

    Source/Objects/Box.h
    Source/Objects/BVH.cpp
    Source/Objects/BVH.h
    Source/Objects/ConstantMedium.cpp
    Source/Objects/ConstantMedium.h
    Source/Objects/Hittable.cpp
    Source/Objects/Hittable.h
    Source/Objects/HittableList.cpp
    Source/Objects/HittableList.h
    Source/Objects/Quad.cpp
    Source/Objects/Quad.h
    Source/Objects/RotateY.cpp
    Source/Objects/RotateY.h
    Source/Objects/Sphere.cpp
    Source/Objects/Sphere.h
    Source/Objects/Translate.cpp
    Source/Objects/Translate.h

    Source/Render/Camera.cpp
    Source/Render/Camera.h
    Source/Render/CheckerTexture.cpp
    Source/Render/CheckerTexture.h
    Source/Render/Color.cpp
    Source/Render/Color.h
    Source/Render/Dielectric.cpp
    Source/Render/Dielectric.h
    Source/Render/DiffuseLight.cpp
    Source/Render/DiffuseLight.h
    Source/Render/HitRecord.cpp
    Source/Render/HitRecord.h
    Source/Render/ImageTexture.cpp
    Source/Render/ImageTexture.h
    Source/Render/Isotropic.cpp
    Source/Render/Isotropic.h
    Source/Render/Lambertian.cpp
    Source/Render/Lambertian.h
    Source/Render/Material.cpp
    Source/Render/Material.h
    Source/Render/Metal.cpp
    Source/Render/Metal.h
    Source/Render/NoiseTexture.cpp
    Source/Render/NoiseTexture.h
    Source/Render/NoiseTextureCamouflage.cpp
    Source/Render/NoiseTextureCamouflage.h
    Source/Render/NoiseTextureSmooth.cpp
    Source/Render/NoiseTextureSmooth.h
    Source/Render/PerlinNoise.cpp
    Source/Render/PerlinNoise.h
    Source/Render/Ray.cpp
    Source/Render/Ray.h
    Source/Render/Renderer.cpp
    Source/Render/Renderer.h
    Source/Render/ScatterRecord.cpp
    Source/Render/ScatterRecord.h
    Source/Render/SolidColor.cpp
    Source/Render/SolidColor.h
    Source/Render/Texture.cpp
    Source/Render/Texture.h

    Source/UI/ImGuiHelper.cpp
    Source/UI/ImGuiHelper.h
    Source/UI/Layer.cpp
    Source/UI/Layer.h

    Source/Utils/Filesystem.cpp
    Source/Utils/Filesystem.h
    Source/Utils/Log.cpp
    Source/Utils/Log.h
    Source/Utils/Random.cpp
    Source/Utils/Random.h
    Source/Utils/StringTools.cpp
    Source/Utils/StringTools.h
    Source/Utils/Timers.cpp
    Source/Utils/Timers.h
    Source/Utils/VulkanException.cpp
    Source/Utils/VulkanException.h

    # # Scenes
    Source/Scenes/Scene.cpp
    Source/Scenes/Scene.h

    # Official scenes
    Source/Scenes/RTWeekOneDefaultScene.cpp
    Source/Scenes/RTWeekOneFinalScene.cpp
    Source/Scenes/RTWeekOneTestScene.cpp
    Source/Scenes/RTWeekNextCornellBoxScene.cpp
    Source/Scenes/RTWeekNextCornellSmokeScene.cpp
    Source/Scenes/RTWeekNextDefaultScene.cpp
    Source/Scenes/RTWeekNextEarthScene.cpp
    Source/Scenes/RTWeekNextFinalScene.cpp
    Source/Scenes/RTWeekNextQuadsScene.cpp
    Source/Scenes/RTWeekNextRandomSpheresScene.cpp
    Source/Scenes/RTWeekNextSimpleLightScene.cpp
    Source/Scenes/RTWeekNextTwoPerlinSpheresScene.cpp
    Source/Scenes/RTWeekNextTwoSpheresScene.cpp
    Source/Scenes/RTWeekRestACornellBoxScene.cpp
    Source/Scenes/RTWeekRestBCornellBoxMirrorScene.cpp
    Source/Scenes/RTWeekRestCCornellBoxGlassScene.cpp

    # Custom scenes
    Source/Scenes/CornellBoxLightsScene.cpp
    Source/Scenes/WhiteSperesScene.cpp

    Source/Vulkan/Application.cpp
    Source/Vulkan/Application.h
    Source/Vulkan/Image.cpp
    Source/Vulkan/Image.h

    # Shaders
    ${SHADER_FILES}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Find glslc executable
find_program(GLSLC_EXECUTABLE NAMES glslc)

if(NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found. Please make sure Vulkan SDK is installed and available in your system's PATH.")
endif()

# Copy resources to the build directory.
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(isMultiConfig) # In Miti-Config mode, we need to copy to the correct configuration directory.
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/DummyResources")
        message(FATAL_ERROR "\"${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/DummyResources\" exists, remove it!")
    endif()

    add_custom_target(
        always_run_target ALL
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

        get_filename_component(SHADER_FILE_NAME ${SHADER_FILE} NAME)
        set(SHADER_OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Resources/Shaders/${SHADER_FILE_NAME}.spv")
        add_custom_command(
            OUTPUT
            "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Dummy${SHADER_FILE_NAME}" # fake! ensure we run!
            "${SHADER_OUTPUT_FILE}"
            COMMAND ${GLSLC_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}" -o "${SHADER_OUTPUT_FILE}"
            DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}"
        )
        list(APPEND SHADER_OUTPUT_FILES "${SHADER_OUTPUT_FILE}")
    endforeach()

    # Dummy target to ensure that resources are up-to-date
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/DummyShaders")
        message(FATAL_ERROR "\"${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/DummyShaders\" exists, remove it!")
    endif()

    add_custom_target(
        always_run_shaders_target ALL
        DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/DummyShaders" # fake! ensure we run!
        ${SHADER_OUTPUT_FILES}
    )
else() # In single-config mode, we just copy to the build directory.
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/DummyResources")
        message(FATAL_ERROR "\"${CMAKE_CURRENT_BINARY_DIR}/DummyResources\" exists, remove it!")
    endif()

    add_custom_target(
        always_run_target ALL
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

        get_filename_component(SHADER_FILE_NAME ${SHADER_FILE} NAME)
        set(SHADER_OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/Resources/Shaders/${SHADER_FILE_NAME}.spv")
        add_custom_command(
            OUTPUT
            "${CMAKE_CURRENT_BINARY_DIR}/Dummy${SHADER_FILE_NAME}" # fake! ensure we run!
            "${SHADER_OUTPUT_FILE}"
            COMMAND ${GLSLC_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}" -o "${SHADER_OUTPUT_FILE}"
            DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}"
        )
        list(APPEND SHADER_OUTPUT_FILES "${SHADER_OUTPUT_FILE}")
    endforeach()

    # Dummy target to ensure that resources are up-to-date
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/DummyShaders")
        message(FATAL_ERROR "\"${CMAKE_CURRENT_BINARY_DIR}/DummyShaders\" exists, remove it!")
    endif()

    add_custom_target(
        always_run_shaders_target ALL
        DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/DummyShaders" # fake! ensure we run!
        ${SHADER_OUTPUT_FILES}
    )
endif()

find_package(glfw3 REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog REQUIRED)
find_package(Stb REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME} ${EXE_PLATFORM_KEYWORD} ${SOURCE_FILES})
add_dependencies(${PROJECT_NAME} always_run_target)

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
    imgui::imgui
    spdlog::spdlog
    Vulkan::Vulkan
)

target_include_directories(${PROJECT_NAME} PRIVATE "Source" ${Stb_INCLUDE_DIR})

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

if(VERBOSE_OUTPUT)
    cmake_list_all_variables()
endif()
