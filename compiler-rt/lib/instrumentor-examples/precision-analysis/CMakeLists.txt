# CMakeLists.txt for Precision Analysis Example
#
# This example demonstrates analyzing whether floating-point operations
# could use lower precision while maintaining acceptable accuracy using
# the Instrumentor pass.

add_compiler_rt_component(precision-analysis)

set(PRECISION_ANALYSIS_SOURCES
  precision_analysis_runtime.cpp
  )

set(PRECISION_ANALYSIS_HEADERS
  precision_analysis_runtime.h
  )

# Include paths for instrumentor runtime headers
# The instrumentor runtime headers are in llvm/utils
include_directories(${COMPILER_RT_SOURCE_DIR}/../llvm/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Common flags
set(PRECISION_ANALYSIS_CFLAGS
  ${COMPILER_RT_COMMON_CFLAGS}
  -std=c++17
  )

# Determine supported architectures
if(APPLE)
  # On Darwin, use the darwin OSX architectures
  set(PRECISION_ANALYSIS_SUPPORTED_ARCH arm64)
  if(NOT CMAKE_OSX_ARCHITECTURES STREQUAL "")
    set(PRECISION_ANALYSIS_SUPPORTED_ARCH ${CMAKE_OSX_ARCHITECTURES})
  endif()
  if(DARWIN_osx_ARCHS)
    set(PRECISION_ANALYSIS_SUPPORTED_ARCH ${DARWIN_osx_ARCHS})
  endif()
else()
  # For non-Apple platforms, use the default target architecture
  set(PRECISION_ANALYSIS_SUPPORTED_ARCH ${COMPILER_RT_DEFAULT_TARGET_ARCH})
endif()

message(STATUS "Precision Analysis supported architectures: ${PRECISION_ANALYSIS_SUPPORTED_ARCH}")

# Build the static runtime library for Apple platforms
if(APPLE)
  add_compiler_rt_runtime(clang_rt.precision_analysis
    STATIC
    OS osx
    ARCHS ${PRECISION_ANALYSIS_SUPPORTED_ARCH}
    CFLAGS ${PRECISION_ANALYSIS_CFLAGS}
    SOURCES ${PRECISION_ANALYSIS_SOURCES}
    ADDITIONAL_HEADERS ${PRECISION_ANALYSIS_HEADERS}
    PARENT_TARGET precision-analysis)
else()
  add_compiler_rt_runtime(clang_rt.precision_analysis
    STATIC
    ARCHS ${PRECISION_ANALYSIS_SUPPORTED_ARCH}
    CFLAGS ${PRECISION_ANALYSIS_CFLAGS}
    SOURCES ${PRECISION_ANALYSIS_SOURCES}
    ADDITIONAL_HEADERS ${PRECISION_ANALYSIS_HEADERS}
    PARENT_TARGET precision-analysis)
endif()

# Install the configuration file as a resource
install(FILES precision_analysis_config.json
        DESTINATION share/llvm/instrumentor-configs
        COMPONENT precision-analysis)
