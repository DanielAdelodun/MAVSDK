cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(lights)

add_executable(lights lights.cpp)
add_executable(fly_lit_mission fly_lit_mission.cpp)

add_library(colormap colormap.cpp)

find_package(MAVSDK REQUIRED)

target_link_libraries(lights MAVSDK::mavsdk colormap)
target_link_libraries(fly_lit_mission MAVSDK::mavsdk colormap)

if(NOT MSVC)
    add_compile_options(lights PRIVATE -Wall -Wextra)
else()
    add_compile_options(lights PRIVATE -WX -W2)
endif()
