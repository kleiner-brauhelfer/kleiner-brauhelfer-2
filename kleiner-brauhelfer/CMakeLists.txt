cmake_minimum_required(VERSION 3.16)

# Define project organization, name, version and build type of the kleiner-brauhelfer-2 application
set(ORGANIZATION kleiner-brauhelfer)
set(TARGET kleiner-brauhelfer-2)
set(VERSION 2.6.5)
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
qt_standard_project_setup(REQUIRES 6.10)

# Globally disable static linkage for Qt6
set(Qt6_STATIC OFF CACHE BOOL "" FORCE)

# Set required C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Handle rcc automatically for Qt targets
set(CMAKE_AUTORCC ON)

# Source files
set(SOURCES
    main.cpp
    mainwindow.cpp
    settings.cpp
    tababfuellen.cpp
    tabbraudaten.cpp
    tabgaerverlauf1.cpp
    tabgaerverlauf2.cpp
    tabrezept.cpp
    tabsudauswahl.cpp
    tabsudauswahl_sudinfo.cpp
    templatetags.cpp
    charts/chart3.cpp
    charts/chartabfuelldaten.cpp
    charts/chartausbeute.cpp
    charts/chartbase.cpp
    charts/chartbraudaten.cpp
    charts/charthopfen.cpp
    charts/chartmaischplan.cpp
    charts/chartmalz.cpp
    charts/chartrestextrakt.cpp
    charts/qcustomplot.cpp
    commands/setmodeldatacommand.cpp
    commands/undostack.cpp
    dialogs/dlgabout.cpp
    dialogs/dlgabstract.cpp
    dialogs/dlgausruestung.cpp
    dialogs/dlgbewertungen.cpp
    dialogs/dlgbrauuebersicht.cpp
    dialogs/dlgcheckupdate.cpp
    dialogs/dlgdatabasecleaner.cpp
    dialogs/dlgdatenbank.cpp
    dialogs/dlgeinstellungen.cpp
    dialogs/dlgetikett.cpp
    dialogs/dlghilfe.cpp
    dialogs/dlgimportexport.cpp
    dialogs/dlgmaischplanmalz.cpp
    dialogs/dlgphmalz.cpp
    dialogs/dlgprintout.cpp
    dialogs/dlgrestextrakt.cpp
    dialogs/dlgrichtexteditor.cpp
    dialogs/dlgrohstoffe.cpp
    dialogs/dlgrohstoffauswahl.cpp
    dialogs/dlgrohstoffeabziehen.cpp
    dialogs/dlgrohstoffvorlage.cpp
    dialogs/dlgsudteilen.cpp
    dialogs/dlgtableview.cpp
    dialogs/dlguebernahmerezept.cpp
    dialogs/dlgverdampfung.cpp
    dialogs/dlgvolumen.cpp
    dialogs/dlgwasseraufbereitung.cpp
    dialogs/dlgwasserprofile.cpp
    helper/htmlhighlighter.cpp
    helper/mustache.cpp
    helper/obrama.cpp
    model/checkboxdelegate.cpp
    model/comboboxdelegate.cpp
    model/datedelegate.cpp
    model/datetimedelegate.cpp
    model/doublespinboxdelegate.cpp
    model/dsvtablemodel.cpp
    model/ebcdelegate.cpp
    model/ingredientnamedelegate.cpp
    model/linklabeldelegate.cpp
    model/phmalzdelegate.cpp
    model/ratingdelegate.cpp
    model/restextraktdelegate.cpp
    model/sudnamedelegate.cpp
    model/sudwochedelegate.cpp
    model/spinboxdelegate.cpp
    model/tagglobaldelegate.cpp
    model/textdelegate.cpp
    widgets/checkbox.cpp
    widgets/combobox.cpp
    widgets/dateedit.cpp
    widgets/datetimeedit.cpp
    widgets/docktitlebar.cpp
    widgets/doublespinbox.cpp
    widgets/doublespinboxsud.cpp
    widgets/filterbuttonlager.cpp
    widgets/filterbuttonsud.cpp
    widgets/lineedit.cpp
    widgets/proxystyle.cpp
    widgets/pushbutton.cpp
    widgets/radiobutton.cpp
    widgets/spinbox.cpp
    widgets/spinboxsud.cpp
    widgets/svgview.cpp
    widgets/tableview.cpp
    widgets/timeedit.cpp
    widgets/toolbutton.cpp
    widgets/wdgabstractproxy.cpp
    widgets/wdganhang.cpp
    widgets/wdgbemerkung.cpp
    widgets/wdghefegabe.cpp
    widgets/wdghopfengabe.cpp
    widgets/wdgmalzgabe.cpp
    widgets/wdgrast.cpp
    widgets/wdgrating.cpp
    widgets/wdgsudstatus.cpp
    widgets/wdgwasseraufbereitung.cpp
    widgets/wdgweiterezutatgabe.cpp
    widgets/webview.cpp
    widgets/wdgwebvieweditable.cpp
    widgets/widgetdecorator.cpp
    mainwindow.ui
    tababfuellen.ui
    tabbraudaten.ui
    tabgaerverlauf1.ui
    tabgaerverlauf2.ui
    tabrezept.ui
    tabsudauswahl.ui
    dialogs/dlgabout.ui
    dialogs/dlgausruestung.ui
    dialogs/dlgbewertungen.ui
    dialogs/dlgbrauuebersicht.ui
    dialogs/dlgcheckupdate.ui
    dialogs/dlgdatabasecleaner.ui
    dialogs/dlgdatenbank.ui
    dialogs/dlgeinstellungen.ui
    dialogs/dlgetikett.ui
    dialogs/dlghilfe.ui
    dialogs/dlgimportexport.ui
    dialogs/dlgmaischplanmalz.ui
    dialogs/dlgphmalz.ui
    dialogs/dlgprintout.ui
    dialogs/dlgrestextrakt.ui
    dialogs/dlgrichtexteditor.ui
    dialogs/dlgrohstoffe.ui
    dialogs/dlgrohstoffauswahl.ui
    dialogs/dlgrohstoffeabziehen.ui
    dialogs/dlgrohstoffvorlage.ui
    dialogs/dlgsudteilen.ui
    dialogs/dlgtableview.ui
    dialogs/dlguebernahmerezept.ui
    dialogs/dlgverdampfung.ui
    dialogs/dlgvolumen.ui
    dialogs/dlgwasseraufbereitung.ui
    dialogs/dlgwasserprofile.ui
    widgets/wdganhang.ui
    widgets/wdgbemerkung.ui
    widgets/wdghefegabe.ui
    widgets/wdghopfengabe.ui
    widgets/wdgmalzgabe.ui
    widgets/wdgrast.ui
    widgets/wdgrating.ui
    widgets/wdgwasseraufbereitung.ui
    widgets/wdgweiterezutatgabe.ui
    widgets/wdgwebvieweditable.ui
    data.qrc
    images.qrc
)

# Header files
set(HEADERS
    definitionen.h
    mainwindow.h
    settings.h
    tababfuellen.h
    tabbraudaten.h
    tabgaerverlauf1.h
    tabgaerverlauf2.h
    tabrezept.h
    tabsudauswahl.h
    templatetags.h
    charts/chart3.h
    charts/chartabfuelldaten.h
    charts/chartausbeute.h
    charts/chartbase.h
    charts/chartbraudaten.h
    charts/charthopfen.h
    charts/chartmaischplan.h
    charts/chartmalz.h
    charts/chartrestextrakt.h
    charts/qcustomplot.h
    commands/setmodeldatacommand.h
    commands/undostack.h
    dialogs/dlgabout.h
    dialogs/dlgabstract.h
    dialogs/dlgabstract-inl.h
    dialogs/dlgausruestung.h
    dialogs/dlgbewertungen.h
    dialogs/dlgbrauuebersicht.h
    dialogs/dlgcheckupdate.h
    dialogs/dlgdatabasecleaner.h
    dialogs/dlgdatenbank.h
    dialogs/dlgeinstellungen.h
    dialogs/dlgetikett.h
    dialogs/dlghilfe.h
    dialogs/dlgimportexport.h
    dialogs/dlgmaischplanmalz.h
    dialogs/dlgphmalz.h
    dialogs/dlgprintout.h
    dialogs/dlgrestextrakt.h
    dialogs/dlgrichtexteditor.h
    dialogs/dlgrohstoffe.h
    dialogs/dlgrohstoffauswahl.h
    dialogs/dlgrohstoffeabziehen.h
    dialogs/dlgrohstoffvorlage.h
    dialogs/dlgsudteilen.h
    dialogs/dlgtableview.h
    dialogs/dlguebernahmerezept.h
    dialogs/dlgverdampfung.h
    dialogs/dlgvolumen.h
    dialogs/dlgwasseraufbereitung.h
    dialogs/dlgwasserprofile.h
    helper/htmlhighlighter.h
    helper/mustache.h
    helper/obrama.h
    model/checkboxdelegate.h
    model/comboboxdelegate.h
    model/datedelegate.h
    model/datetimedelegate.h
    model/doublespinboxdelegate.h
    model/dsvtablemodel.h
    model/ebcdelegate.h
    model/ingredientnamedelegate.h
    model/linklabeldelegate.h
    model/phmalzdelegate.h
    model/ratingdelegate.h
    model/restextraktdelegate.h
    model/sudnamedelegate.h
    model/sudwochedelegate.h
    model/spinboxdelegate.h
    model/tagglobaldelegate.h
    model/textdelegate.h
    widgets/checkbox.h
    widgets/combobox.h
    widgets/dateedit.h
    widgets/datetimeedit.h
    widgets/docktitlebar.h
    widgets/doublespinbox.h
    widgets/doublespinboxsud.h
    widgets/filterbuttonlager.h
    widgets/filterbuttonsud.h
    widgets/lineedit.h
    widgets/proxystyle.h
    widgets/pushbutton.h
    widgets/radiobutton.h
    widgets/spinbox.h
    widgets/spinboxsud.h
    widgets/svgview.h
    widgets/tableview.h
    widgets/timeedit.h
    widgets/toolbutton.h
    widgets/wdgabstractproxy.h
    widgets/wdganhang.h
    widgets/wdgbemerkung.h
    widgets/wdghefegabe.h
    widgets/wdghopfengabe.h
    widgets/wdgmalzgabe.h
    widgets/wdgrast.h
    widgets/wdgrating.h
    widgets/wdgsudstatus.h
    widgets/wdgwasseraufbereitung.h
    widgets/wdgweiterezutatgabe.h
    widgets/webview.h
    widgets/wdgwebvieweditable.h
    widgets/widgetdecorator.h
)

# Search all translation files
set(TRANSLATIONS
    translations/kbh_en.ts
    translations/kbh_nl.ts
    translations/kbh_sv.ts
)

# Add the target to the build system
qt_add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
)

if(WIN32)
    configure_file(application.rc.in ${CMAKE_CURRENT_BINARY_DIR}/application.rc @ONLY)
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/application.rc)
elseif(APPLE)
    set_source_files_properties(icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    target_sources(${PROJECT_NAME} PRIVATE icon.icns)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE icon.icns
        MACOSX_BUNDLE_GUI_IDENTIFIER com.github.kleiner-brauhelfer.kleiner-brauhelfer-2
        MACOSX_BUNDLE_BUNDLE_NAME "Kleiner Brauhelfer 2"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    )
endif()

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
    QT_DEPRECATED_WARNINGS
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
qt_add_translations(${PROJECT_NAME}
    TS_FILES ${TRANSLATIONS}
    RESOURCE_PREFIX "/translations"
)
qt_add_resources(${PROJECT_NAME} "qtbase_translations"
    PREFIX "/translations"
    BASE "${CMAKE_CURRENT_SOURCE_DIR}/translations"
    FILES
        translations/qtbase_de.qm
        translations/qtbase_en.qm
        translations/qtbase_nl.qm
        translations/qtbase_sv.qm
)

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
