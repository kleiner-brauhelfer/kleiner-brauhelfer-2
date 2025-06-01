cmake_minimum_required(VERSION 3.16)

# Define project organization, name, version and build type of the kleiner-brauhelfer-core library
set(ORGANIZATION kleiner-brauhelfer)
set(TARGET kleiner-brauhelfer-2)
set(VERSION 2.9.2)
set(BUILD_TYPE STATIC)

# Set name, version and language of the project
project(${TARGET} VERSION ${VERSION} LANGUAGES CXX)

# Qt6 requirements
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Sql
    Gui
    Widgets
    SvgWidgets
    Xml
    PrintSupport
    Network
    WebEngineWidgets
    LinguistTools
)

# Setup project-wide defaults to a standard arrangement
qt_standard_project_setup(REQUIRES 6.5)

# Globally disable static linkage for Qt6
set(Qt6_STATIC OFF CACHE BOOL "" FORCE)

# Set required C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Handle rcc automatically for Qt targets
set(CMAKE_AUTORCC ON)

# Source files
file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc
)

# Header files
file(GLOB_RECURSE HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# Search all translation files
file(GLOB TRANSLATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/translations/*.ts
)

# Add the target to the build system
qt_add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

# Add directories to the include search path
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/helper
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
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

# Set the definition of the kleiner-brauhelfer-core library build type
if(${BUILD_TYPE} STREQUAL STATIC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE KLEINERBRAUHELFERCORE_LIBRARY_STATIC)
elseif(${BUILD_TYPE} STREQUAL SHARED)
else()
    message(FATAL_ERROR "BUILD_TYPE must be either STATIC or SHARED" )
endif()

# Link the core and Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    kleiner-brauhelfer-core
    Qt6::Core
    Qt6::Sql
    Qt6::Gui
    Qt6::Widgets
    Qt6::SvgWidgets
    Qt6::Xml
    Qt6::PrintSupport
    Qt6::Network
    Qt6::WebEngineWidgets
)

# Add and build translations
qt_add_translation(${PROJECT_NAME} ${TRANSLATIONS})

# Deployment and installation
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
