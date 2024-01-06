cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(lights)

add_executable(lights
    lights.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(lights
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(lights PRIVATE -Wall -Wextra)
else()
    add_compile_options(lights PRIVATE -WX -W2)
endif()
