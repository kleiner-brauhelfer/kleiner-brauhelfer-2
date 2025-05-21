cmake_minimum_required(VERSION  3.18.0)

# Define base project and version information
project(kleiner-brauhelfer-2 VERSION 2.0.0 LANGUAGES C CXX)

# Globally disable static linkage for Qt6
set(Qt6_STATIC OFF CACHE BOOL "" FORCE)

# We require the C++ 11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add sub-projects, the ordering here matters!
add_subdirectory(kleiner-brauhelfer-core)
add_subdirectory(kleiner-brauhelfer)