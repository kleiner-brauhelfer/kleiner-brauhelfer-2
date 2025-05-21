# Qt6 Requirements
find_package(Qt6 REQUIRED COMPONENTS Core Sql Xml)

# Configure Auto MOC,UIC and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Set the version numbers for the core
set(KBH_VER_MAJ 2)
set(KBH_VER_MIN 9)
set(KBH_VER_PAT 5)

# Search all source files
file(GLOB LIB_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Define the new static library
add_library(kleiner-brauhelfer-core STATIC
        ${LIB_SOURCES}
)

# Add the current directory as include search path
target_include_directories(kleiner-brauhelfer-core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compile definitions
target_compile_definitions(kleiner-brauhelfer-core PRIVATE
        KLEINERBRAUHELFERCORE_LIBRARY_STATIC
        VER_MAJ=${KBH_VER_MAJ}
        VER_MIN=${KBH_VER_MIN}
        VER_PAT=${KBH_VER_PAT}
)

# Link the QT libraries
target_link_libraries(kleiner-brauhelfer-core PUBLIC
        Qt6::Core
        Qt6::Sql
        Qt6::Xml
)

target_compile_options(kleiner-brauhelfer-core PRIVATE
        -Wall
)

