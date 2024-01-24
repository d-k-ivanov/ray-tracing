# Ray Tracer

Ray-Tracing Framework and Visualizer.

This work heavily inspired by:

- [Yan Chernikov](http://www.youtube.com/@TheCherno) and his [Walnut Application](https://github.com/StudioCherno/Walnut)
- [Peter Shirley](https://www.petershirley.com/) and [Ray Tracing in One Weekend series](https://raytracing.github.io/)

## Compilation: Windows

- [Install Visual Studio](https://visualstudio.microsoft.com/downloads/)
- [Install Git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git)
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
