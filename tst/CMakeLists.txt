cmake_minimum_required(VERSION 3.1)
project(test_svm)

if (UNIX)
    set(CMAKE_CXX_COMPILER g++-10)
endif (UNIX)
if (WIN32)
  message([WARNING] if you cant compile you might need a c++20 compaitble windows compiler..?)
endif (WIN32)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compilation stuff
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -ggdb -std=c++20")
set(CMAKE_VERBOSE_MAKEFILE ON)

# Source files
set(PROJECT_SOURCES main.cpp)

# setting up for using pkgconfig includes
#   find_package(PkgConfig REQUIRED)
#   message("pkg-config executeable: " ${PKG_CONFIG_EXECUTABLE})
#
#   # Include the Arc Systems
#   pkg_check_modules(ARCSYSTEMS REQUIRED IMPORTED_TARGET ArcSystems)
#   if(${ARCSYSTEMS_FOUND})
#       MESSAGE("ArcSystems lib: " ${ARCSYSTEMS_LIBRARIES})
#       MESSAGE("ArcSystems links: " ${ARCSYSTEMS_LINK_LIBRARIES})
#   endif()

# Build the program
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} 
                                      -I/usr/include/c++/10
                                      -I/usr/include/x86_64-linux-gnu/c++/10
                                      #${ARCSYSTEMS_LIBRARIES}
                                      m dl
                                      #pthread
)
