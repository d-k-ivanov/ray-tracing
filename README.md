# Ray-Tracing Frameworks and Visualizers

## Acknowledgments

My work is significantly inspired by:

- [Dr. Carlos Ureña Almagro](https://lsi.ugr.es/informacion/directorio-personal/carlos-urena-almagro) from the University of Granada. I'm deeply grateful for his level of support and knowledge, which allowed me to build this project and opened me to the world of Ray Tracing.
- [Peter Shirley](https://www.petershirley.com/), [Steve Hollasch](https://github.com/hollasch), [Trevor David Black](https://github.com/trevordblack) and [Ray Tracing in One Weekend series](https://raytracing.github.io/)
- [Yan Chernikov](http://www.youtube.com/@TheCherno) and his [Walnut Application](https://github.com/StudioCherno/Walnut)
- [Tanguy Fautré](https://github.com/GPSnoopy) and his [RayTracingInVulkan](https://github.com/GPSnoopy/RayTracingInVulkan)

## Compilation: Windows

- [Install Visual Studio](https://visualstudio.microsoft.com/downloads/)
- [Install Git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git)
- [Install Vulkan SDK](https://vulkan.lunarg.com/sdk/home)
- [Install VCPKG](https://vcpkg.io/en/getting-started.html)
- Add VCPKG folder to PATH:
  - Option 1: Command Line tools

    ```bat
    REM Assuming that VCPKG cloned and bootstrapped in c:\src\vcpkg
    REM setx for the global environment, set for the local
    setx PATH c:\src\vcpkg;%PATH%
    set  PATH c:\src\vcpkg;%PATH%
    ```

  - Option 2: PowerShell

    ```ps1
    # Assuming that VCPKG cloned and bootstrapped in c:\src\vcpkg
    [Environment]::SetEnvironmentVariable("PATH", "c:\src\vcpkg;${PATH}", "Machine")
    Set-Item -Path Env:PATH -Value "c:\src\vcpkg;${PATH}"
    ```

  - Option 3: Manually in *System Properties* → *Environment Variables*
- Run `build.bat`
- Open `build\ray-tracing.sln` in Visual Studio to work with the source code

## Compilation: Linux

- [Install Git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git)
- [Install Vulkan SDK](https://vulkan.lunarg.com/sdk/home)
- [Install VCPKG](https://vcpkg.io/en/getting-started.html)
- Add VCPKG folder to PATH:
  - Option 1: Temporary local environment

    ```bash
    # Assuming that VCPKG cloned and bootstrapped in ~/vcpkg
    export PATH="~/vcpkg;${PATH}"
    ```

  - Option 2: Local environment and Bash profile

    ```bash
    # Assuming that VCPKG cloned and bootstrapped in c:\src\vcpkg
    export PATH="~/vcpkg;${PATH}"
    echo 'export PATH="~/vcpkg;${PATH}"' >> ~/.bashrc
    ```

  - **Important Note:** the VCPKG requests installation of additional packages
- Run `build.sh`

## References

- [D3D12 RayTracing Samples](https://github.com/Microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12Raytracing)
- [Fast and Fun: My First Real-Time Ray Tracing Demo](https://devblogs.nvidia.com/my-first-ray-tracing-demo/)
- [George Ouzounoudis' vk_exp](https://github.com/georgeouzou/vk_exp)
- [Getting Started with RTX Ray Tracing](https://github.com/NVIDIAGameWorks/GettingStartedWithRTXRayTracing)
- [Introduction to Real-Time Ray Tracing with Vulkan](https://devblogs.nvidia.com/vulkan-raytracing)
- [Khronos Vulkan Registry](https://www.khronos.org/registry/vulkan/)
- [NVIDIA Converting VK_NV_ray_tracing to VK_KHR_ray_tracing](https://nvpro-samples.github.io/vk_raytracing_tutorial_KHR/NV_to_KHR.md.htm)
- [NVIDIA Vulkan Forums](https://devtalk.nvidia.com/default/board/166/vulkan)
- [NVIDIA Vulkan Ray Tracing Helpers: Introduction](https://developer.nvidia.com/rtx/raytracing/vkray_helpers)
- [NVIDIA Vulkan Ray Tracing Tutorial (VK_KHR_ray_tracing)](https://github.com/nvpro-samples/vk_raytracing_tutorial_KHR)
- [NVIDIA Vulkan Ray Tracing Tutorial](https://developer.nvidia.com/rtx/raytracing/vkray)
- [Profiling DXR shaders with Timer Instrumentation](https://www.reddit.com/r/vulkan/comments/hhyeyj/profiling_dxr_shaders_with_timer_instrumentation/)
- [Ray Tracing in One Weekend series](https://raytracing.github.io/)
- [Vulkan Ray Tracing Final Specification Release](https://www.khronos.org/blog/vulkan-ray-tracing-final-specification-release)
- [Vulkan Tutorial](https://vulkan-tutorial.com/)
