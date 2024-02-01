# Ray Tracer

Ray-Tracing Framework and Visualizer.

## Acknowledgment

My work is significantly inspired by [Dr. Carlos Ureña Almagro](https://lsi.ugr.es/informacion/directorio-personal/carlos-urena-almagro) from the University of Granada. I'm deeply grateful for his level of support and knowledge, which allowed me to build this project and opened me to the world of Ray Tracing.

The initial application part is inspired by:

- [Peter Shirley](https://www.petershirley.com/), [Steve Hollasch](https://github.com/hollasch), [Trevor David Black](https://github.com/trevordblack) and [Ray Tracing in One Weekend series](https://raytracing.github.io/)
- [Yan Chernikov](http://www.youtube.com/@TheCherno) and his [Walnut Application](https://github.com/StudioCherno/Walnut)

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
