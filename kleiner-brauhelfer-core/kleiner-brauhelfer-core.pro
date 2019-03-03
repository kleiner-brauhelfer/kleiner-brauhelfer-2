QT += core sql
QT -= gui

ORGANIZATION = kleiner-brauhelfer
TARGET = kleiner-brauhelfer-core

# Hauptversionsnummer
VER_MAJ = 1
# Datenbankversion
VER_MIN = 24
# Patchversion
VER_PAT = 0

VERSION = $$sprintf("%1.%2.%3", $$VER_MAJ, $$VER_MIN, $$VER_PAT)
DEFINES += VER_MAJ=\"$$VER_MAJ\" VER_MIN=\"$$VER_MIN\" VER_PAT=\"$$VER_PAT\"

TEMPLATE = lib
DEFINES += ORGANIZATION=\\\"$$ORGANIZATION\\\" TARGET=\\\"$$TARGET\\\" VERSION=\\\"$$VERSION\\\"
DEFINES += KLEINERBRAUHELFERCORE_LIBRARY
CONFIG += skip_target_version_ext unversioned_libname unversioned_soname

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
    modelausruestung.cpp \
    modelbewertungen.cpp \
    modelflaschenlabeltags.cpp \
    modelhauptgaerverlauf.cpp \
    modelhefe.cpp \
    modelhopfen.cpp \
    modelhopfengaben.cpp \
    modelmalz.cpp \
    modelmalzschuettung.cpp \
    modelnachgaerverlauf.cpp \
    modelrasten.cpp \
    modelschnellgaerverlauf.cpp \
    modelsud.cpp \
    modelwasser.cpp \
    modelweiterezutaten.cpp \
    modelweiterezutatengaben.cpp \
    proxymodel.cpp \
    proxymodelrohstoff.cpp \
    proxymodelsud.cpp \
    sqltablemodel.cpp \
    sudobject.cpp

HEADERS += \
    kleiner-brauhelfer-core_global.h \
    biercalc.h \
    brauhelfer.h \
    database.h \
    database_defs.h \
    kleiner-brauhelfer-core_global.h \
    modelausruestung.h \
    modelbewertungen.h \
    modelflaschenlabeltags.h \
    modelhauptgaerverlauf.h \
    modelhefe.h \
    modelhopfen.h \
    modelhopfengaben.h \
    modelmalz.h \
    modelmalzschuettung.h \
    modelnachgaerverlauf.h \
    modelrasten.h \
    modelschnellgaerverlauf.h \
    modelsud.h \
    modelwasser.h \
    modelweiterezutaten.h \
    modelweiterezutatengaben.h \
    proxymodel.h \
    proxymodelrohstoff.h \
    proxymodelsud.h \
    sqltablemodel.h \
    sudobject.h
