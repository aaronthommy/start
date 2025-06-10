cmake_minimum_required(VERSION 3.24)
project(jump-run LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)          # für IntelliSense

# ------------------------------------------------------------
#  Abhängigkeiten
# ------------------------------------------------------------
find_package(raylib 5 REQUIRED)                 # Raylib

find_path(NLOHMANN_JSON_INCLUDE_DIR
          NAMES nlohmann/json.hpp
          PATHS /usr/local/include /opt/homebrew/include $ENV{HOME}/.local/include)
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${NLOHMANN_JSON_INCLUDE_DIR})

# ------------------------------------------------------------
#  Quellen –  *ohne* CMakeFiles-Unterordner
# ------------------------------------------------------------
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

#  CompilerId-Dateien aussortieren:
list(FILTER SRC EXCLUDE REGEX "/CMakeFiles/")

add_executable(jump-run ${SRC})

# ------------------------------------------------------------
#  Include-Pfade
# ------------------------------------------------------------
target_include_directories(jump-run PRIVATE
    include
    include/core
    external/raygui              # enthält raygui.h
    ${NLOHMANN_JSON_INCLUDE_DIR}
)

# ------------------------------------------------------------
#  Link-Libs
# ------------------------------------------------------------
target_link_libraries(jump-run PRIVATE
    raylib
    nlohmann_json
)

# ------------------------------------------------------------
#  Assets & Daten kopieren
# ------------------------------------------------------------
add_custom_command(TARGET jump-run POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:jump-run>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data
            $<TARGET_FILE_DIR:jump-run>/data
)


file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Stelle sicher, dass keine build/CMakeFiles-Unterordner gematcht werden:
list(FILTER SRC EXCLUDE REGEX "/CMakeFiles/")