cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME ray-tracing-cpu)
project(${PROJECT_NAME} VERSION ${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}.${APP_VERSION_TWEAK} LANGUAGES CXX)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Resources/ray-tracing.rc.in" "${CMAKE_CURRENT_BINARY_DIR}/ray-tracing.rc")

set(SHADER_FILES
    # Shaders will be here
)

set(SOURCE_FILES
    Main.cpp
    # ${CMAKE_CURRENT_BINARY_DIR}/ray-tracing.rc

    Engine/Application.cpp
    Engine/Application.h
    Engine/Image.cpp
    Engine/Image.h

    Math/AABB.cpp
    Math/AABB.h
    Math/Constants.cpp
    Math/Constants.h
    Math/Converters.cpp
    Math/Converters.h
    Math/CosinePDF.cpp
    Math/CosinePDF.h
    Math/HittablePDF.cpp
    Math/HittablePDF.h
    Math/Interval.cpp
    Math/Interval.h
    Math/MixturePDF.cpp
    Math/MixturePDF.h
    Math/ONB.cpp
    Math/ONB.h
    Math/PDF.cpp
    Math/PDF.h
    Math/SpherePDF.cpp
    Math/SpherePDF.h
    Math/Vector3.cpp
    Math/Vector3.h

    Objects/Box.h
    Objects/BVH.cpp
    Objects/BVH.h
    Objects/ConstantMedium.cpp
    Objects/ConstantMedium.h
    Objects/Hittable.cpp
    Objects/Hittable.h
    Objects/HittableList.cpp
    Objects/HittableList.h
    Objects/Quad.cpp
    Objects/Quad.h
    Objects/RotateY.cpp
    Objects/RotateY.h
    Objects/Sphere.cpp
    Objects/Sphere.h
    Objects/Translate.cpp
    Objects/Translate.h

    Render/Camera.cpp
    Render/Camera.h
    Render/CheckerTexture.cpp
    Render/CheckerTexture.h
    Render/Color.cpp
    Render/Color.h
    Render/Dielectric.cpp
    Render/Dielectric.h
    Render/DiffuseLight.cpp
    Render/DiffuseLight.h
    Render/HitRecord.cpp
    Render/HitRecord.h
    Render/ImageTexture.cpp
    Render/ImageTexture.h
    Render/Isotropic.cpp
    Render/Isotropic.h
    Render/Lambertian.cpp
    Render/Lambertian.h
    Render/Material.cpp
    Render/Material.h
    Render/Metal.cpp
    Render/Metal.h
    Render/NoiseTexture.cpp
    Render/NoiseTexture.h
    Render/NoiseTextureCamouflage.cpp
    Render/NoiseTextureCamouflage.h
    Render/NoiseTextureSmooth.cpp
    Render/NoiseTextureSmooth.h
    Render/PerlinNoise.cpp
    Render/PerlinNoise.h
    Render/Ray.cpp
    Render/Ray.h
    Render/Renderer.cpp
    Render/Renderer.h
    Render/ScatterRecord.cpp
    Render/ScatterRecord.h
    Render/SolidColor.cpp
    Render/SolidColor.h
    Render/Texture.cpp
    Render/Texture.h

    UI/ImGuiHelper.cpp
    UI/ImGuiHelper.h
    UI/Layer.cpp
    UI/Layer.h

    Utils/Filesystem.cpp
    Utils/Filesystem.h
    Utils/Log.cpp
    Utils/Log.h
    Utils/Random.cpp
    Utils/Random.h
    Utils/StringTools.cpp
    Utils/StringTools.h
    Utils/Timers.cpp
    Utils/Timers.h
    Utils/VulkanException.cpp
    Utils/VulkanException.h

    # # Scenes
    Scenes/Scene.cpp
    Scenes/Scene.h

    # Official scenes
    Scenes/RTWeekOneDefaultScene.cpp
    Scenes/RTWeekOneFinalScene.cpp
    Scenes/RTWeekOneTestScene.cpp
    Scenes/RTWeekNextCornellBoxScene.cpp
    Scenes/RTWeekNextCornellSmokeScene.cpp
    Scenes/RTWeekNextDefaultScene.cpp
    Scenes/RTWeekNextEarthScene.cpp
    Scenes/RTWeekNextFinalScene.cpp
    Scenes/RTWeekNextQuadsScene.cpp
    Scenes/RTWeekNextRandomSpheresScene.cpp
    Scenes/RTWeekNextSimpleLightScene.cpp
    Scenes/RTWeekNextTwoPerlinSpheresScene.cpp
    Scenes/RTWeekNextTwoSpheresScene.cpp
    Scenes/RTWeekRestACornellBoxScene.cpp
    Scenes/RTWeekRestBCornellBoxMirrorScene.cpp
    Scenes/RTWeekRestCCornellBoxGlassScene.cpp

    # Custom scenes
    Scenes/CornellBoxLightsScene.cpp
    Scenes/WhiteSperesScene.cpp

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
        always_run_target_cpu ALL
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
        always_run_shaders_target_cpu ALL
        DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/DummyShaders" # fake! ensure we run!
        ${SHADER_OUTPUT_FILES}
    )
else() # In single-config mode, we just copy to the build directory.
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/DummyResources")
        message(FATAL_ERROR "\"${CMAKE_CURRENT_BINARY_DIR}/DummyResources\" exists, remove it!")
    endif()

    add_custom_target(
        always_run_target_cpu ALL
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
        always_run_shaders_target_cpu ALL
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
add_dependencies(${PROJECT_NAME} always_run_target_cpu)

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

target_include_directories(${PROJECT_NAME} PRIVATE "." ${Stb_INCLUDE_DIR})
