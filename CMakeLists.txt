cmake_minimum_required(VERSION 3.15)

project(afm)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

################################################################################
# Options

option(ENABLE_MALLOC_COUNT "Keep track of allocations with malloc_count."   OFF)
option(ENABLE_MIMALLOC "Use mimalloc if installed."                         OFF)
option(BUILD_TESTS "Build unit test."                                       OFF)
option(BUILD_BENCHMARKS "Build benchmarks."                                 OFF)

################################################################################
# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug"
FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

################################################################################
# Version number
SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "1")
SET(VERSION_PATCH "0")
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(PROJECT_VERSION ${VERSION})

execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "Git current branch: ${GIT_BRANCH}")
message(STATUS "Git commit hash: ${GIT_COMMIT_HASH}")

message(STATUS "Generating version.cpp")
configure_file(
        ${PROJECT_SOURCE_DIR}/src/version.cpp.in
        ${PROJECT_BINARY_DIR}/generated/version.cpp)

################################################################################
# Compiler and linker flags

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(BUILD_TYPE_FLAGS "-O3")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TYPE_FLAGS "-g")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(BUILD_TYPE_FLAGS "-O3 -g -DNDEBUG")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BUILD_TYPE_FLAGS}")


################################################################################
# Thirdparty

include(FetchContent)

# sdsl
FetchContent_Declare(
        sdsl
        GIT_REPOSITORY https://github.com/simongog/sdsl-lite
)
FetchContent_GetProperties(sdsl)
if(NOT sdsl_POPULATED)
    FetchContent_Populate(sdsl)
    set(GENERATE_DOC OFF CACHE BOOL "Do not generate doxygen for sdsl-lite")
    add_subdirectory(${sdsl_SOURCE_DIR} ${sdsl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# divsuffsort
FetchContent_Declare(
        divsufsort
        GIT_REPOSITORY https://github.com/simongog/libdivsufsort.git
        GIT_TAG        2.0.1
)
FetchContent_GetProperties(divsufsort)
if(NOT divsufsort_POPULATED)
    FetchContent_Populate(divsufsort)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Do not build a shared library for libdivsufsort")
    set(BUILD_EXAMPLES OFF CACHE BOOL "Do not build libdivsufsort example")
    set(BUILD_DIVSUFSORT64 ON CACHE BOOL "Build libdivsufsort in 64-bits mode")
    add_subdirectory(${divsufsort_SOURCE_DIR} ${divsufsort_BINARY_DIR}  EXCLUDE_FROM_ALL)
    target_include_directories(divsufsort PUBLIC "${divsufsort_BINARY_DIR}/include")
    target_include_directories(divsufsort64 PUBLIC "${divsufsort_BINARY_DIR}/include")
endif()

# malloc count
FetchContent_Declare(
        malloc_count
        GIT_REPOSITORY https://github.com/bingmann/malloc_count.git
        GIT_TAG        v0.7.1
)
FetchContent_GetProperties(malloc_count)
if(NOT malloc_count_POPULATED)
    FetchContent_Populate(malloc_count)
    add_library(malloc_count OBJECT ${malloc_count_SOURCE_DIR}/malloc_count.c ${malloc_count_SOURCE_DIR}/malloc_count.h)
    target_include_directories(malloc_count PUBLIC  "${malloc_count_SOURCE_DIR}")
    target_link_libraries(malloc_count dl)
endif()

# Catch
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1
)
# Making available only if building tests
FetchContent_GetProperties(Catch2)
if (NOT Catch2_POPULATED AND (BUILD_TESTS OR ${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    FetchContent_MakeAvailable(Catch2)
endif()

# Hayai Benchmarks
FetchContent_Declare(
        hayai
        GIT_REPOSITORY https://github.com/nickbruun/hayai.git
        GIT_TAG        v1.0.2
)
FetchContent_GetProperties(hayai)
if(NOT hayai_POPULATED)
    FetchContent_Populate(hayai)
endif()

# SPDLOG
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.10.0
)
FetchContent_GetProperties(spdlog)
if(NOT spdlog_POPULATED)
    FetchContent_Populate(spdlog)
endif()
include_directories(${spdlog_SOURCE_DIR}/include)

# CLI11
FetchContent_Declare(
        cli11
        GIT_REPOSITORY "https://github.com/CLIUtils/CLI11.git"
        GIT_TAG "v1.9.0"
)
FetchContent_GetProperties(cli11)
if(NOT cli11_POPULATED)
    FetchContent_Populate(cli11)
endif()
include_directories(${cli11_SOURCE_DIR}/include)

# Openmp
find_package(OpenMP)
if(OpenMP_FOUND OR OPENMP_FOUND)
    message(STATUS "HAVE_OPENMP: OpenMP")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(OMP_LIBRARIES "${OpenMP_CXX_LIBRARIES}")
elseif(APPLE AND EXISTS "/opt/local/lib/libomp")
    # official Apple compiler with macports' libomp
    message(STATUS "HAVE_OPENMP: OpenMP[MacPorts]")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xpreprocessor -fopenmp -I/opt/local/include/libomp")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -I/opt/local/include/libomp")
    set(OMP_LIBRARIES "-L/opt/local/lib/libomp -lomp")
elseif(APPLE AND EXISTS "/usr/local/opt/libomp")
    # official Apple compiler with homebrew's kegg libomp
    message(STATUS "HAVE_OPENMP: OpenMP[Homebrew kegg]")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include")
    set(OMP_LIBRARIES "-L/usr/local/opt/libomp/lib -lomp")
elseif(APPLE AND EXISTS "/usr/local/lib/libomp.dylib")
    # official Apple compiler with homebrew's libomp
    message(STATUS "HAVE_OPENMP: OpenMP[Homebrew]")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xpreprocessor -fopenmp")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp")
    set(OMP_LIBRARIES "-L/usr/local/lib -lomp")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND EXISTS  "/usr/local/lib64/libgomp.a")
    # custom static openmp for official linux builds
    message(STATUS "HAVE_OPENMP: OpenMP[static]")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xpreprocessor -fopenmp")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp")
    set(OMP_LIBRARIES "/usr/local/lib64/libgomp.a")
else()
    message(STATUS "HAVE_OPENMP: NOT FOUND")
endif()

FetchContent_Declare(
        kseq
        GIT_REPOSITORY https://github.com/lh3/seqtk.git
        GIT_TAG        v1.3
)
if(NOT kseq_POPULATED)
    FetchContent_Populate(kseq)
endif()
include_directories(${kseq_SOURCE_DIR})

# MiMalloc
if (ENABLE_MIMALLOC)
    set(MI_BUILD_STATIC OFF)
    set(MI_BUILD_OBJECT OFF)
    set(MI_BUILD_TESTS  OFF)
    FetchContent_Declare(
            mimalloc
            GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
            GIT_TAG        v2.0.9
    )
    FetchContent_GetProperties(mimalloc)
    if (NOT mimalloc_POPULATED)
        FetchContent_MakeAvailable(mimalloc)
        message(STATUS "Building mimalloc. It will be dynamically linked.")
    endif()
    set(MIMALLOC_D_LIB mimalloc)
endif()

################################################################################
# Lib

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_BINARY_DIR}/generated)

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

if (ENABLE_MIMALLOC AND ENABLE_MALLOC_COUNT)
    message(STATUS "mimalloc enabled, disabling malloc count")
    set(ENABLE_MALLOC_COUNT FALSE)
endif()

if (ENABLE_MALLOC_COUNT)
    message(STATUS "Using malloc_count")
    set(MALLOC_COUNT_LIBRARIES malloc_count)
else()
    set(MALLOC_COUNT_LIBRARIES "")
endif()

set(AFM_LIBS ${OMP_LIBRARIES} ${MALLOC_COUNT_LIBRARIES}
${MIMALLOC_D_LIB})

################################################################################
# Tests

if (BUILD_TESTS OR ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    message(STATUS "Building unit tests")

    add_executable(afm_unit_tests tests/unit_tests.cpp)
    target_link_libraries(afm_unit_tests Catch2::Catch2 sdsl divsufsort divsufsort64 rlestring ${AFM_LIBS})

    enable_testing()
    add_test(NAME afm_unit_tests COMMAND afm_unit_tests)
endif()

################################################################################
# Benchmarks

if (BUILD_BENCHMARKS OR ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    message(STATUS "Building benchmarks")

    add_executable(afm_benchmarks benchmarks/benchmarks.cpp)
    target_link_libraries(afm_benchmarks hayai sdsl divsufsort divsufsort64 rlestring ${AFM_LIBS})
endif()

################################################################################
# Targets

file(GLOB SOURCES_GENERATED ${PROJECT_BINARY_DIR}/generated/*.cpp)

add_executable(afm afm.cpp ${SOURCES_GENERATED})
target_link_libraries(afm dl sdsl divsufsort divsufsort64 ${AFM_LIBS})

################################################################################
# Install

install(TARGETS afm)

################################################################################
# CPack

set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_PACKAGE_VERSION "${VERSION}")

include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "STGZ;TGZ;DEB")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_PACKAGE_VENDOR "University of Florida")
set(CPACK_PACKAGE_CONTACT "marco.oliva@ufl.edu")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "AFM")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-sources")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Marco Oliva")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEB_COMPONENT_INSTALL YES)
include(CPack)





