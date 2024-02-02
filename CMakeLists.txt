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
include(ToolchainPostUpdates)

project(${PROJECT_NAME})

set(EXE_PLATFORM_KEYWORD "")
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(EXE_PLATFORM_KEYWORD "")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(EXE_PLATFORM_KEYWORD "MACOSX_BUNDLE")
endif()

set(SOURCE_FILES
    Source/Main.cpp

    Source/Math/AABB.cpp
    Source/Math/AABB.h
    Source/Math/Constants.h
    Source/Math/Converters.cpp
    Source/Math/Converters.h
    Source/Math/CosinePDF.cpp
    Source/Math/Interval.cpp
    Source/Math/Interval.h
    Source/Math/ONB.cpp
    Source/Math/ONB.h
    Source/Math/PDF.cpp
    Source/Math/PDF.h
    Source/Math/Vector3.cpp
    Source/Math/Vector3.h
   Source/Math/CosinePDF.h
   Source/Math/HittablePDF.cpp
   Source/Math/HittablePDF.h
   Source/Math/MixturePDF.cpp
   Source/Math/MixturePDF.h
   Source/Math/SpherePDF.cpp
   Source/Math/SpherePDF.h

    Source/Objects/BVH.cpp
    Source/Objects/BVH.h
    Source/Objects/Box.h
    Source/Objects/ConstantMedium.cpp
    Source/Objects/ConstantMedium.h
    Source/Objects/Hittable.cpp
    Source/Objects/Hittable.h
    Source/Objects/HittableList.cpp
    Source/Objects/HittableList.h
    Source/Objects/Quad.cpp
    Source/Objects/Quad.h
    Source/Objects/Sphere.cpp
    Source/Objects/Sphere.h

    Source/Render/Camera.cpp
    Source/Render/Camera.h
    Source/Render/Color.h
    Source/Render/HitRecord.cpp
    Source/Render/HitRecord.h
    Source/Render/Material.cpp
    Source/Render/Material.h
    Source/Render/PerlinNoise.cpp
    Source/Render/PerlinNoise.h
    Source/Render/Ray.cpp
    Source/Render/Ray.h
    Source/Render/Renderer.cpp
    Source/Render/Renderer.h
    Source/Render/Texture.cpp
    Source/Render/Texture.h

    Source/Scenes/Scene.cpp
    Source/Scenes/Scene.h
    Source/Scenes/RTWeekOneDefaultScene.cpp
    Source/Scenes/RTWeekOneFinalScene.cpp
    Source/Scenes/RTWeekOneTestScene.cpp
    Source/Scenes/RTWeekNextDefaultScene.cpp
    Source/Scenes/RTWeekNextRandomSpheresScene.cpp
    Source/Scenes/RTWeekNextTwoSpheresScene.cpp
    Source/Scenes/RTWeekNextEarthScene.cpp
    Source/Scenes/RTWeekNextTwoPerlinSpheresScene.cpp
    Source/Scenes/RTWeekNextQuadsScene.cpp
    Source/Scenes/RTWeekNextSimpleLightScene.cpp
    Source/Scenes/RTWeekNextCornellBoxScene.cpp
    Source/Scenes/RTWeekNextCornellSmokeScene.cpp
    Source/Scenes/RTWeekNextFinalScene.cpp
    Source/Scenes/RTWeekRestACornellBoxScene.cpp
    Source/Scenes/RTWeekRestBCornellBoxMirrorScene.cpp
    Source/Scenes/RTWeekRestCCornellBoxGlassScene.cpp

    Source/UI/Application.cpp
    Source/UI/Application.h
    Source/UI/Image.cpp
    Source/UI/Image.h
    Source/UI/ImGuiHelper.cpp
    Source/UI/ImGuiHelper.h
    Source/UI/Layer.h

    Source/Utils/Filesystem.cpp
    Source/Utils/Filesystem.h
    Source/Utils/Log.cpp
    Source/Utils/Log.h
    Source/Utils/Random.cpp
    Source/Utils/Random.h
    Source/Utils/StringTools.h
    Source/Utils/Timers.cpp
    Source/Utils/Timers.h
    Source/Utils/VulkanException.cpp
    Source/Utils/VulkanException.h
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Dummy")
    message(FATAL_ERROR "\"${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Dummy\" exists, remove it!")
endif()

add_custom_target(
    always_run_target ALL
    DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Dummy" # fake! ensure we run!
)

add_custom_command(
    OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Dummy" # fake! ensure we run!
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Resources"
)

find_package(glfw3 REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog REQUIRED)
find_package(Stb REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_dependencies(${PROJECT_NAME} always_run_target)

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
