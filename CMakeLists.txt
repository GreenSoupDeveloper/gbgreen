cmake_minimum_required(VERSION 3.10)
project(gbgreen VERSION 1.0)

# Set C++ standard (adjust if needed, e.g., C++11/14/17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Place binaries in the 'bin' folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Source files (excluding tempCodeRunnerFile.c)
file(GLOB SOURCES "src/*.cpp")
set(HEADERS "src/*.h" "src/*.hpp")
set(RESOURCES "src/*.qrc" "src/*.ui")
set(RESOURCES_FILES "src/*.png" "src/*.jpg" "src/*.svg" "src/*.wav" "src/*.mp3")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)
# Create executable
add_executable(gbgreen ${SOURCES})
# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

# Optional: Add compiler warnings (adjust for your compiler)
if(MSVC)
    target_compile_options(gbgreen PRIVATE /W4)
else()
    target_compile_options(gbgreen PRIVATE -Wall -Wextra)
endif()

# Optional: If you need MIDI library (e.g., RtMidi)
# find_package(RtMidi REQUIRED)
# target_link_libraries(gbgreen RtMidi)
# Link to the actual SDL3 library.
target_link_libraries(gbgreen PRIVATE SDL3::SDL3)