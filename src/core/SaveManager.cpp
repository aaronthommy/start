// src/core/SaveManager.cpp
#include "core/SaveManager.h"
#include <filesystem>   // C++17
#include <cstdlib>

namespace fs = std::filesystem;

// --------------------------------------------------
//   Hilfsfunktion: Speicherort
// --------------------------------------------------
std::string SaveManager::path() const
{
#ifdef _WIN32
    const char* appData = std::getenv("APPDATA");
    fs::path dir = appData ? fs::path(appData) / "JumpRun" : fs::temp_directory_path();
#else   // macOS & Linux → $HOME/.local/share
    const char* home = std::getenv("HOME");
    fs::path dir = home ? fs::path(home) / ".local/share/jump-run"
                        : fs::temp_directory_path();
#endif
    fs::create_directories(dir);       // existiert sonst nicht
    return (dir / "save.json").string();
}
