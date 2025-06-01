cmake_minimum_required(VERSION 3.16)

# Define project organization, name, version and build type of the library
set(ORGANIZATION kleiner-brauhelfer)
set(TARGET kleiner-brauhelfer-core)
set(VERSION 2.9.5)
set(BUILD_TYPE STATIC)

# Set name, version and language of the project
project(${TARGET} VERSION ${VERSION} LANGUAGES CXX)

# Qt6 requirements
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Sql
    Xml
)

# Setup project-wide defaults to a standard arrangement
qt_standard_project_setup(REQUIRES 6.5)

# Globally disable static linkage for Qt6
set(Qt6_STATIC OFF CACHE BOOL "" FORCE)

# Set required C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enables building multi-ABI packages for Android
set(QT_ANDROID_BUILD_ALL_ABIS TRUE)

# Source files
file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Header files
file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# Add the library to the build system
qt_add_library(${PROJECT_NAME} ${BUILD_TYPE}
    ${SOURCES}
    ${HEADERS}
)

# Add directories to the include search path
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set the definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    ORGANIZATION="${ORGANIZATION}"
    TARGET="${TARGET}"
    VER_MAJ=${PROJECT_VERSION_MAJOR}
    VER_MIN=${PROJECT_VERSION_MINOR}
    VER_PAT=${PROJECT_VERSION_PATCH}
    VERSION="${PROJECT_VERSION}"
)

# Set the definition of the library build type
if(${BUILD_TYPE} STREQUAL STATIC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE KLEINERBRAUHELFERCORE_LIBRARY_STATIC)
elseif(${BUILD_TYPE} STREQUAL SHARED)
    target_compile_definitions(${PROJECT_NAME} PRIVATE KLEINERBRAUHELFERCORE_LIBRARY_SHARED)
else()
    message(FATAL_ERROR "BUILD_TYPE must be either STATIC or SHARED" )
endif()

# Link the Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Sql
    Qt6::Xml
)
