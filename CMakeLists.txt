cmake_minimum_required(VERSION 3.18)

project(evopath
    VERSION 0.1.0
    DESCRIPTION "Algorytm ewolucyjny do planowania sciezki robota"
    LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Opcje
# ---------------------------------------------------------------------------
option(EVO_ENABLE_OPENMP "Backend OpenMP"                       ON)
option(EVO_BUILD_PYTHON  "Modul Pythona (pybind11)"             ON)
option(EVO_BUILD_APPS    "Programy z katalogu apps/"            ON)
option(EVO_BUILD_TESTS   "Testy C++ z katalogu tests/cpp/"      ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Typ budowania" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # dla clangd / IDE

# ---------------------------------------------------------------------------
# Biblioteka rdzenia: core/ + backend CPU (+ OpenMP)
# ---------------------------------------------------------------------------
file(GLOB_RECURSE EVO_CORE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/core/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backends/cpu/*.cpp)

add_library(evo_core STATIC ${EVO_CORE_SOURCES})
add_library(evopath::core ALIAS evo_core)

target_include_directories(evo_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/backends)

# PIC, bo biblioteka statyczna trafia do modulu Pythona (.so)
set_target_properties(evo_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_options(evo_core PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Wpedantic>)

if(EVO_ENABLE_OPENMP)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        file(GLOB_RECURSE EVO_OMP_SOURCES CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/backends/openmp/*.cpp)
        target_sources(evo_core PRIVATE ${EVO_OMP_SOURCES})
        target_link_libraries(evo_core PUBLIC OpenMP::OpenMP_CXX)
        target_compile_definitions(evo_core PUBLIC EVO_HAS_OPENMP=1)
    else()
        message(STATUS "Nie znaleziono OpenMP - backend OpenMP pominiety")
    endif()
endif()


# ---------------------------------------------------------------------------
# Programy pomocnicze (apps/*.cpp -> osobny plik wykonywalny kazdy)
# ---------------------------------------------------------------------------
if(EVO_BUILD_APPS)
    file(GLOB EVO_APP_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/apps/*.cpp)
    foreach(src ${EVO_APP_SOURCES})
        get_filename_component(name ${src} NAME_WE)
        add_executable(${name} ${src})
        target_link_libraries(${name} PRIVATE evopath::core)
    endforeach()
endif()

# ---------------------------------------------------------------------------
# Testy C++ (tests/cpp/*.cpp -> osobny test kazdy, uruchamiane przez ctest)
# ---------------------------------------------------------------------------
if(EVO_BUILD_TESTS)
    enable_testing()
    file(GLOB EVO_TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp/*.cpp)
    foreach(src ${EVO_TEST_SOURCES})
        get_filename_component(name ${src} NAME_WE)
        add_executable(${name} ${src})
        target_link_libraries(${name} PRIVATE evopath::core)
        add_test(NAME ${name} COMMAND ${name}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endforeach()
endif()

# ---------------------------------------------------------------------------
# Modul Pythona: import evopath  (pakiet python/evopath + _core.so)
# ---------------------------------------------------------------------------
if(EVO_BUILD_PYTHON)
    find_package(Python 3.8 REQUIRED COMPONENTS Interpreter Development.Module)
    find_package(pybind11 CONFIG REQUIRED)

    file(GLOB EVO_BINDING_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/bindings/*.cpp)
    pybind11_add_module(_core ${EVO_BINDING_SOURCES})
    target_link_libraries(_core PRIVATE evopath::core)
    target_compile_definitions(_core PRIVATE EVO_VERSION="${PROJECT_VERSION}")

    if(SKBUILD)
        # Budowanie przez `pip install .` (scikit-build-core)
        install(TARGETS _core LIBRARY DESTINATION evopath)
    else()
        # Zwykle budowanie CMake: modul obok pakietu, do pracy bez instalacji
        # (PYTHONPATH=python)
        set_target_properties(_core PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python/evopath)
    endif()
endif()

# ---------------------------------------------------------------------------
# Podsumowanie konfiguracji
# ---------------------------------------------------------------------------
message(STATUS "evopath ${PROJECT_VERSION} (${CMAKE_BUILD_TYPE})")
message(STATUS "  OpenMP : ${OpenMP_CXX_FOUND}")
message(STATUS "  Python : ${EVO_BUILD_PYTHON}")
message(STATUS "  Testy  : ${EVO_BUILD_TESTS}")
