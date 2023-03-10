cmake_minimum_required(VERSION 3.5.0)

# Specify project name
project(rt_viewer)

# Specify build type
set(CMAKE_BUILD_TYPE RelWithDebInfo)

# Add project source directories
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/src" PROJECT_SRCS)

# Add project include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

# Define variable for linked libraries
set(PROJECT_LIBRARIES)

# OpenGL
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
  include_directories(SYSTEM ${OPENGL_INCLUDE_DIR})
  set(PROJECT_LIBRARIES ${PROJECT_LIBRARIES} ${OPENGL_LIBRARIES})
endif(OPENGL_FOUND)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/external/glfw" ${CMAKE_CURRENT_BINARY_DIR}/glfw)
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include")

# GLEW (used for OpenGL function loading)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/external/glew/src" PROJECT_SRCS)
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/external/glew/include")
add_definitions(-DGLEW_STATIC -DGLEW_NO_GLU)

# GLM (used for math functions and matrix/vector types)
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/external/glm")

# lodepng (used for image I/O)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/external/lodepng" PROJECT_SRCS)
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/external/lodepng")

# Dear ImGui (used for GUI)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/external/imgui" PROJECT_SRCS)
include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")

# Set extra compiler flags
if(UNIX AND NOT APPLE)
  set(CMAKE_CXX_FLAGS "-W -Wall -std=c++11 -fopenmp")
endif(UNIX AND NOT APPLE)
if(APPLE)
  set(CMAKE_CXX_FLAGS "-W -Wall -std=c++11 -ObjC++")
endif(APPLE)

# Create build files for application
add_executable(${PROJECT_NAME} ${PROJECT_SRCS} src/rt_extra.h src/rt_material.h)

# Link against libraries
target_link_libraries(${PROJECT_NAME} glfw ${PROJECT_LIBRARIES} ${GLFW_LIBRARIES})

# Install application
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
