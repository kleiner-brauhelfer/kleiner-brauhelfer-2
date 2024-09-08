QT += core sql xml
QT -= gui

ORGANIZATION = kleiner-brauhelfer
TARGET = kleiner-brauhelfer-core

# Hauptversionsnummer
VER_MAJ = 2
# Datenbankversion
VER_MIN = 9
# Patchversion
VER_PAT = 5

VERSION = $$sprintf("%1.%2.%3", $$VER_MAJ, $$VER_MIN, $$VER_PAT)
DEFINES += VER_MAJ=\"$$VER_MAJ\" VER_MIN=\"$$VER_MIN\" VER_PAT=\"$$VER_PAT\"

TEMPLATE = lib
CONFIG += staticlib
DEFINES += ORGANIZATION=\\\"$$ORGANIZATION\\\" TARGET=\\\"$$TARGET\\\" VERSION=\\\"$$VERSION\\\"
CONFIG += skip_target_version_ext unversioned_libname unversioned_soname
staticlib {
  DEFINES += KLEINERBRAUHELFERCORE_LIBRARY_STATIC
} else {
  DEFINES += KLEINERBRAUHELFERCORE_LIBRARY_SHARED
}

CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += warn_on

!android: DESTDIR = $$OUT_PWD/../bin
OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp

SOURCES += \
    biercalc.cpp \
    brauhelfer.cpp \
    database.cpp \
    database_update.cpp \
    importexport.cpp \
    modelanhang.cpp \
    modelausruestung.cpp \
    modelbewertungen.cpp \
    modeletiketten.cpp \
    modelgeraete.cpp \
    modelhauptgaerverlauf.cpp \
    modelhefe.cpp \
    modelhefegaben.cpp \
    modelhopfen.cpp \
    modelhopfengaben.cpp \
    modelkategorien.cpp \
    modelmaischplan.cpp \
    modelmalz.cpp \
    modelmalzschuettung.cpp \
    modelnachgaerverlauf.cpp \
    modelschnellgaerverlauf.cpp \
    modelsud.cpp \
    modeltags.cpp \
    modelwasser.cpp \
    modelwasseraufbereitung.cpp \
    modelweiterezutaten.cpp \
    modelweiterezutatengaben.cpp \
    proxymodel.cpp \
    proxymodelrohstoff.cpp \
    proxymodelsud.cpp \
    sqltablemodel.cpp \
    sudobject.cpp

HEADERS += \
    importexport.h \
    kleiner-brauhelfer-core_global.h \
    biercalc.h \
    brauhelfer.h \
    database.h \
    kleiner-brauhelfer-core_global.h \
    modelanhang.h \
    modelausruestung.h \
    modelbewertungen.h \
    modeletiketten.h \
    modelgeraete.h \
    modelhauptgaerverlauf.h \
    modelhefe.h \
    modelhefegaben.h \
    modelhopfen.h \
    modelhopfengaben.h \
    modelkategorien.h \
    modelmaischplan.h \
    modelmalz.h \
    modelmalzschuettung.h \
    modelnachgaerverlauf.h \
    modelschnellgaerverlauf.h \
    modelsud.h \
    modeltags.h \
    modelwasser.h \
    modelwasseraufbereitung.h \
    modelweiterezutaten.h \
    modelweiterezutatengaben.h \
    proxymodel.h \
    proxymodelrohstoff.h \
    proxymodelsud.h \
    sqltablemodel.h \
    sudobject.h
