# Qt6 Requirements
find_package(Qt6 REQUIRED COMPONENTS Core Sql Gui Widgets SvgWidgets Xml PrintSupport Network WebEngineWidgets LinguistTools)
#qt_standard_project_setup()

# Configure Auto MOC,UIC and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Set the version numbers
set(KBH_VER_MAJ 2)
set(KBH_VER_MIN 6)
set(KBH_VER_PAT 3)

set(ORGANIZATION kleiner-brauhelfer)
set(TARGET kleiner-brauhelfer-2)

# Search all source files
file(GLOB_RECURSE EXEC_SOURCES
        *.cpp
        *.ui
        *.qrc
)
# Search all translation files
file(GLOB_RECURSE EXEC_TRANSLATIONS
    *.ts
)

# Add the target
add_executable(kleiner-brauhelfer
        ${EXEC_SOURCES}
)

# Add the named directories as include search paths
target_include_directories(kleiner-brauhelfer PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
        ${CMAKE_CURRENT_SOURCE_DIR}/model
        ${CMAKE_CURRENT_SOURCE_DIR}/helper
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets
)

# Link the core library and QT
target_link_libraries(kleiner-brauhelfer PRIVATE
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

# Compile Definitions
target_compile_definitions(kleiner-brauhelfer PRIVATE
        VER_MAJ="${KBH_VER_MAJ}"
        VER_MIN="${KBH_VER_MIN}"
        VER_PAT="${KBH_VER_PAT}"
        ORGANIZATION="${ORGANIZATION}"
        TARGET="${TARGET}"
        VERSION="${KBH_VER_MAJ}.${KBH_VERS_MIN}.${KBH_VER_PAT}"
)

# Add and build translations
qt_add_translation(qmFiles ${EXEC_TRANSLATIONS})

# Deployment/Installation
install(TARGETS kleiner-brauhelfer
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
        TARGET kleiner-brauhelfer
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})