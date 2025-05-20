!versionAtLeast(QT_VERSION, 6.5.0):error("Use at least Qt 6.5.0")

QT += core sql gui widgets svg svgwidgets xml
QT += printsupport
QT += network
QT += webenginewidgets

ORGANIZATION = kleiner-brauhelfer
TARGET = kleiner-brauhelfer-2

VER_MAJ = 2
VER_MIN = 6
VER_PAT = 3
VERSION = $$sprintf("%1.%2.%3", $$VER_MAJ, $$VER_MIN, $$VER_PAT)
DEFINES += VER_MAJ=\"$$VER_MAJ\" VER_MIN=\"$$VER_MIN\" VER_PAT=\"$$VER_PAT\"

TEMPLATE = app
DEFINES += ORGANIZATION=\\\"$$ORGANIZATION\\\" TARGET=\\\"$$TARGET\\\" VERSION=\\\"$$VERSION\\\"

win32:RC_ICONS += icon.ico
macx:ICON = icon.icns

CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += warn_on

DESTDIR = $$OUT_PWD/../bin
OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp

LIBS += -L$$OUT_PWD/../bin/ -lkleiner-brauhelfer-core
INCLUDEPATH += $$PWD/../kleiner-brauhelfer-core
DEPENDPATH += $$PWD/../kleiner-brauhelfer-core
DEFINES += KLEINERBRAUHELFERCORE_LIBRARY_STATIC
win32: PRE_TARGETDEPS += $$OUT_PWD/../bin/kleiner-brauhelfer-core.lib
unix: PRE_TARGETDEPS += $$OUT_PWD/../bin/libkleiner-brauhelfer-core.a

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    settings.cpp \
    tababstract.cpp \
    tababfuellen.cpp \
    tabbewertung.cpp \
    tabbraudaten.cpp \
    tabetikette.cpp \
    tabgaerverlauf.cpp \
    tabrezept.cpp \
    tabsudauswahl.cpp \
    tabsudauswahl_sudinfo.cpp \
    tabzusammenfassung.cpp \
    templatetags.cpp \
    charts/chart3.cpp \
    charts/chartabfuelldaten.cpp \
    charts/chartausbeute.cpp \
    charts/chartbase.cpp \
    charts/chartbraudaten.cpp \
    charts/charthopfen.cpp \
    charts/chartmaischplan.cpp \
    charts/chartmalz.cpp \
    charts/chartrestextrakt.cpp \
    charts/qcustomplot.cpp \
    commands/setmodeldatacommand.cpp \
    commands/undostack.cpp \
    dialogs/dlgabout.cpp \
    dialogs/dlgabstract.cpp \
    dialogs/dlgausruestung.cpp \
    dialogs/dlgbrauuebersicht.cpp \
    dialogs/dlgcheckupdate.cpp \
    dialogs/dlgconsole.cpp \
    dialogs/dlgdatabasecleaner.cpp \
    dialogs/dlgdatenbank.cpp \
    dialogs/dlghilfe.cpp \
    dialogs/dlgimportexport.cpp \
    dialogs/dlgmaischplanmalz.cpp \
    dialogs/dlgmodule.cpp \
    dialogs/dlgphmalz.cpp \
    dialogs/dlgrestextrakt.cpp \
    dialogs/dlgrichtexteditor.cpp \
    dialogs/dlgrohstoffe.cpp \
    dialogs/dlgrohstoffauswahl.cpp \
    dialogs/dlgrohstoffeabziehen.cpp \
    dialogs/dlgrohstoffvorlage.cpp \
    dialogs/dlgsudteilen.cpp \
    dialogs/dlgtableview.cpp \
    dialogs/dlguebernahmerezept.cpp \
    dialogs/dlgverdampfung.cpp \
    dialogs/dlgvolumen.cpp \
    dialogs/dlgwasseraufbereitung.cpp \
    dialogs/dlgwasserprofile.cpp \
    helper/htmlhighlighter.cpp \
    helper/mustache.cpp \
    helper/obrama.cpp \
    model/checkboxdelegate.cpp \
    model/comboboxdelegate.cpp \
    model/datedelegate.cpp \
    model/datetimedelegate.cpp \
    model/doublespinboxdelegate.cpp \
    model/dsvtablemodel.cpp \
    model/ebcdelegate.cpp \
    model/ingredientnamedelegate.cpp \
    model/linklabeldelegate.cpp \
    model/phmalzdelegate.cpp \
    model/proxymodelsudcolored.cpp \
    model/ratingdelegate.cpp \
    model/restextraktdelegate.cpp \
    model/spinboxdelegate.cpp \
    model/textdelegate.cpp \
    widgets/combobox.cpp \
    widgets/dateedit.cpp \
    widgets/datetimeedit.cpp \
    widgets/doublespinbox.cpp \
    widgets/doublespinboxsud.cpp \
    widgets/iconthemed.cpp \
    widgets/pushbutton.cpp \
    widgets/spinbox.cpp \
    widgets/spinboxsud.cpp \
    widgets/svgview.cpp \
    widgets/tableview.cpp \
    widgets/timeedit.cpp \
    widgets/wdgabstractproxy.cpp \
    widgets/wdganhang.cpp \
    widgets/wdgbemerkung.cpp \
    widgets/wdgbewertung.cpp \
    widgets/wdghefegabe.cpp \
    widgets/wdghopfengabe.cpp \
    widgets/wdgmalzgabe.cpp \
    widgets/wdgrast.cpp \
    widgets/wdgrating.cpp \
    widgets/wdgwasseraufbereitung.cpp \
    widgets/wdgweiterezutatgabe.cpp \
    widgets/webview.cpp \
    widgets/wdgwebvieweditable.cpp \
    widgets/widgetdecorator.cpp

HEADERS += \
    definitionen.h \
    mainwindow.h \
    settings.h \
    tababstract.h \
    tababfuellen.h \
    tabbewertung.h \
    tabbraudaten.h \
    tabetikette.h \
    tabgaerverlauf.h \
    tabrezept.h \
    tabsudauswahl.h \
    tabzusammenfassung.h \
    templatetags.h \
    charts/chart3.h \
    charts/chartabfuelldaten.h \
    charts/chartausbeute.h \
    charts/chartbase.h \
    charts/chartbraudaten.h \
    charts/charthopfen.h \
    charts/chartmaischplan.h \
    charts/chartmalz.h \
    charts/chartrestextrakt.h \
    charts/qcustomplot.h \
    commands/setmodeldatacommand.h \
    commands/undostack.h \
    dialogs/dlgabout.h \
    dialogs/dlgabstract.h \
    dialogs/dlgabstract-inl.h \
    dialogs/dlgausruestung.h \
    dialogs/dlgbrauuebersicht.h \
    dialogs/dlgcheckupdate.h \
    dialogs/dlgconsole.h \
    dialogs/dlgdatabasecleaner.h \
    dialogs/dlgdatenbank.h \
    dialogs/dlghilfe.h \
    dialogs/dlgimportexport.h \
    dialogs/dlgmaischplanmalz.h \
    dialogs/dlgmodule.h \
    dialogs/dlgphmalz.h \
    dialogs/dlgrestextrakt.h \
    dialogs/dlgrichtexteditor.h \
    dialogs/dlgrohstoffe.h \
    dialogs/dlgrohstoffauswahl.h \
    dialogs/dlgrohstoffeabziehen.h \
    dialogs/dlgrohstoffvorlage.h \
    dialogs/dlgsudteilen.h \
    dialogs/dlgtableview.h \
    dialogs/dlguebernahmerezept.h \
    dialogs/dlgverdampfung.h \
    dialogs/dlgvolumen.h \
    dialogs/dlgwasseraufbereitung.h \
    dialogs/dlgwasserprofile.h \
    helper/htmlhighlighter.h \
    helper/mustache.h \
    helper/obrama.h \
    model/checkboxdelegate.h \
    model/comboboxdelegate.h \
    model/datedelegate.h \
    model/datetimedelegate.h \
    model/doublespinboxdelegate.h \
    model/dsvtablemodel.h \
    model/ebcdelegate.h \
    model/ingredientnamedelegate.h \
    model/linklabeldelegate.h \
    model/phmalzdelegate.h \
    model/proxymodelsudcolored.h \
    model/ratingdelegate.h \
    model/restextraktdelegate.h \
    model/spinboxdelegate.h \
    model/textdelegate.h \
    widgets/combobox.h \
    widgets/dateedit.h \
    widgets/datetimeedit.h \
    widgets/doublespinbox.h \
    widgets/doublespinboxsud.h \
    widgets/iconthemed.h \
    widgets/pushbutton.h \
    widgets/spinbox.h \
    widgets/spinboxsud.h \
    widgets/svgview.h \
    widgets/tableview.h \
    widgets/timeedit.h \
    widgets/wdgabstractproxy.h \
    widgets/wdganhang.h \
    widgets/wdgbemerkung.h \
    widgets/wdgbewertung.h \
    widgets/wdghefegabe.h \
    widgets/wdghopfengabe.h \
    widgets/wdgmalzgabe.h \
    widgets/wdgrast.h \
    widgets/wdgrating.h \
    widgets/wdgwasseraufbereitung.h \
    widgets/wdgweiterezutatgabe.h \
    widgets/webview.h \
    widgets/wdgwebvieweditable.h \
    widgets/widgetdecorator.h

FORMS += \
    mainwindow.ui \
    tababfuellen.ui \
    tabbewertung.ui \
    tabbraudaten.ui \
    tabetikette.ui \
    tabgaerverlauf.ui \
    tabrezept.ui \
    tabsudauswahl.ui \
    tabzusammenfassung.ui \
    dialogs/dlgabout.ui \
    dialogs/dlgausruestung.ui \
    dialogs/dlgbrauuebersicht.ui \
    dialogs/dlgcheckupdate.ui \
    dialogs/dlgconsole.ui \
    dialogs/dlgdatabasecleaner.ui \
    dialogs/dlgdatenbank.ui \
    dialogs/dlghilfe.ui \
    dialogs/dlgimportexport.ui \
    dialogs/dlgmaischplanmalz.ui \
    dialogs/dlgmodule.ui \
    dialogs/dlgphmalz.ui \
    dialogs/dlgrestextrakt.ui \
    dialogs/dlgrichtexteditor.ui \
    dialogs/dlgrohstoffe.ui \
    dialogs/dlgrohstoffauswahl.ui \
    dialogs/dlgrohstoffeabziehen.ui \
    dialogs/dlgrohstoffvorlage.ui \
    dialogs/dlgsudteilen.ui \
    dialogs/dlgtableview.ui \
    dialogs/dlguebernahmerezept.ui \
    dialogs/dlgverdampfung.ui \
    dialogs/dlgvolumen.ui \
    dialogs/dlgwasseraufbereitung.ui \
    dialogs/dlgwasserprofile.ui \
    widgets/wdganhang.ui \
    widgets/wdgbemerkung.ui \
    widgets/wdgbewertung.ui \
    widgets/wdghefegabe.ui \
    widgets/wdghopfengabe.ui \
    widgets/wdgmalzgabe.ui \
    widgets/wdgrast.ui \
    widgets/wdgrating.ui \
    widgets/wdgwasseraufbereitung.ui \
    widgets/wdgweiterezutatgabe.ui \
    widgets/wdgwebvieweditable.ui

RESOURCES += \
    data.qrc \
    images.qrc \
    translations.qrc

TRANSLATIONS += \
    translations/kbh_en.ts \
    translations/kbh_nl.ts \
    translations/kbh_sv.ts

TR_EXCLUDE += dialogs/dlgabstract-inl.h
