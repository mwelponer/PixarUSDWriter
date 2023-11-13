cmake_minimum_required(VERSION 3.12)
project(PixarUSDWriter)

# set the build variant Degub/Release
set(CMAKE_BUILD_TYPE "Debug")

# Set C++ standard
#set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(USD_LIBRARY_DIRECTORY /opt/local/USD/lib)
set(USD_INCLUDE_DIRECTORY /opt/local/USD/include)


# Add your source files
set(SOURCES
    src/main.cpp
    src/PixarUSDWriter.cpp
    # Add other source files if any
)

#find_library(USD_USD usd HINTS ${USD_LIBRARY_DIRECTORY})

# Add include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${USD_INCLUDE_DIRECTORY} #${PROJECT_SOURCE_DIR}/usd/include
)

# Add USD library directory
link_directories(
    ${USD_LIBRARY_DIRECTORY} #${PROJECT_SOURCE_DIR}/usd/lib
)

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Link against USD libraries
target_link_libraries(${PROJECT_NAME}
    usd_usd
    usd_gf
    usd_usdGeom
    usd_sdf
    usd_tf
    usd_vt
    # Add other USD libraries if needed (e.g., usdGeom, usdShade, etc.)
)

# Set compiler flags for USD
if (MSVC)
    # Add specific compiler options for Visual Studio
    target_compile_options(${PROJECT_NAME} PRIVATE /EHsc)
else()
    # Add specific compiler options for other compilers (GCC, Clang, etc.)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()
