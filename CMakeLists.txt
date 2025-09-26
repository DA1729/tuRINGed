cmake_minimum_required(VERSION 3.16)
project(turinged VERSION 0.1.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Include directories
include_directories(include)

# Collect all source files
file(GLOB_RECURSE TURINGED_SOURCES
    "src/turinged/*.cpp"
)

# Create the static library
add_library(turinged STATIC ${TURINGED_SOURCES})

# Set target include directories
target_include_directories(turinged PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Optional: Create shared library as well
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
if(BUILD_SHARED_LIBS)
    add_library(turinged_shared SHARED ${TURINGED_SOURCES})
    target_include_directories(turinged_shared PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    set_target_properties(turinged_shared PROPERTIES OUTPUT_NAME turinged)
endif()

# Examples
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests
option(BUILD_TESTS "Build test programs" ON)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Install configuration
install(TARGETS turinged
    EXPORT turingedTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/turinged
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Export targets
install(EXPORT turingedTargets
    FILE turingedTargets.cmake
    NAMESPACE turinged::
    DESTINATION lib/cmake/turinged
)

# Create and install config files
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/turingedConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/turingedConfig.cmake"
    INSTALL_DESTINATION lib/cmake/turinged
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/turingedConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/turingedConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/turingedConfigVersion.cmake"
    DESTINATION lib/cmake/turinged
)