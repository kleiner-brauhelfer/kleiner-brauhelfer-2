#!/bin/sh

BUILD_DIR=$1
QT_DIR=$2
SCRIPT_DIR=$(dirname $0)

rm "${BUILD_DIR}/libkleiner-brauhelfer-core.a"

mkdir "${BUILD_DIR}/usr"
mkdir "${BUILD_DIR}/usr/bin"
cp "${BUILD_DIR}/kleiner-brauhelfer" "${BUILD_DIR}/usr/bin"
cp "${BUILD_DIR}/portable" "${BUILD_DIR}/usr/bin"

mkdir "${BUILD_DIR}/usr/share"
mkdir "${BUILD_DIR}/usr/share/applications"
cp "${SCRIPT_DIR}/kleiner-brauhelfer.desktop" "${BUILD_DIR}/usr/share/applications"

mkdir "${BUILD_DIR}/usr/share/icons"
cp "${SCRIPT_DIR}/kleiner-brauhelfer.svg" "${BUILD_DIR}/usr/share/icons"

"${SCRIPT_DIR}/linuxdeployqt-6-x86_64.AppImage" "${BUILD_DIR}/usr/share/applications/kleiner-brauhelfer.desktop" -qmake="${QT_DIR}/qmake" -no-copy-copyright-files
