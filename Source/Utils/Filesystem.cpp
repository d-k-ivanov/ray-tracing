#include "Filesystem.h"

#ifdef _WIN32
#include <Shlwapi.h>
#include <windows.h>
#else
#include <libgen.h>
#include <limits.h>
#include <unistd.h>
#endif

#include <filesystem>

std::string ThisExecutableLocation()
{
#ifdef _WIN32
    char              rawPathName[MAX_PATH];
    const std::string executablePath = {rawPathName, GetModuleFileNameA(nullptr, rawPathName, MAX_PATH)};
#else
    char        rawPathName[PATH_MAX];
    const char* res = realpath("/proc/self/exe", rawPathName);
    if(!res)
    {
        return ".";
    }
    std::string executablePath = {rawPathName};
#endif
    const auto executableDir = weakly_canonical(std::filesystem::path(executablePath)).parent_path();
    return executableDir.string();
}
