cmake_minimum_required(VERSION 3.24)
project(start LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Raylib aus MSYS2-UCRT finden                                       
find_package(raylib 5 REQUIRED)

# --------------------------------------------------------------------
#  Quell-Dateien
# --------------------------------------------------------------------
add_executable(start
    src/main.cpp
    src/menu.cpp
    include/menu.h
)


target_link_libraries(start PRIVATE raylib)

target_include_directories(start PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(start PRIVATE raylib)

# --------------------------------------------------------------------
#  Assets kopieren (Bilder, Fonts, Sounds …)
# --------------------------------------------------------------------
add_custom_command(TARGET start POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:start>/assets)
