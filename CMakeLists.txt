cmake_minimum_required(VERSION 3.16)
project(robot_ga VERSION 0.1.0 LANGUAGES CXX)

option(ROBOT_GA_BUILD_EXE    "Program robot_ga (src/main.cpp)"   ON)
option(ROBOT_GA_BUILD_PYTHON "Modul Pythona robot_ga (pybind11)" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# --- Biblioteka z algorytmem: wszystko z src/ poza main.cpp ---
file(GLOB CORE_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_library(robot_ga_core STATIC ${CORE_SOURCES})
target_include_directories(robot_ga_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_compile_options(robot_ga_core PRIVATE -Wall -Wextra)
set_target_properties(robot_ga_core PROPERTIES POSITION_INDEPENDENT_CODE ON)   # trafia do .so

# --- Program terminalowy ---
if(ROBOT_GA_BUILD_EXE)
    add_executable(robot_ga src/main.cpp)
    target_link_libraries(robot_ga PRIVATE robot_ga_core)
    target_compile_options(robot_ga PRIVATE -Wall -Wextra)
endif()

# --- Modul Pythona: import robot_ga ---
if(ROBOT_GA_BUILD_PYTHON)
    find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development.Module)
    find_package(pybind11 CONFIG REQUIRED)

    pybind11_add_module(_core bindings/module.cpp)
    target_link_libraries(_core PRIVATE robot_ga_core)

    if(SKBUILD)
        # pip install .
        install(TARGETS _core LIBRARY DESTINATION robot_ga)
    else()
        # zwykle budowanie: modul laduje obok pakietu (PYTHONPATH=python)
        set_target_properties(_core PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python/robot_ga)
    endif()
endif()
