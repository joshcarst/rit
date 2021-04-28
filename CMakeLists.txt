cmake_minimum_required(VERSION 3.1)

project(rit VERSION 0.0.0)

include(GNUInstallDirs)

include(cmake/CMakeUtils.cmake)

rit_set_library_prefix("librit_")

find_package(Boost REQUIRED 
  COMPONENTS 
    program_options 
    iostreams 
    filesystem 
    system
)
find_package(Eigen3 REQUIRED NO_MODULE)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_subdirectory(imgs)
