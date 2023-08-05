#!/bin/sh

BUILD_DIR=$1
QT_DIR=$2
SCRIPT_DIR=$(dirname "$(readlink -f "${0}")")

# remove unnecessary stuff
rm "${BUILD_DIR}/libkleiner-brauhelfer-core.a"

# create deb package 
DEB_DIR="${BUILD_DIR}/deb"
mkdir -p "${DEB_DIR}/usr/local/bin"
cp "${BUILD_DIR}/kleiner-brauhelfer-2" "${DEB_DIR}/usr/local/bin"
mkdir -p "${DEB_DIR}/usr/share/pixmaps"
cp "${SCRIPT_DIR}/../../kleiner-brauhelfer-2.svg" "${DEB_DIR}/usr/share/pixmaps/"
mkdir -p "${DEB_DIR}/usr/share/applications"
cp "${SCRIPT_DIR}/deb/kleiner-brauhelfer-2.desktop" "${DEB_DIR}/usr/share/applications/"
mkdir -p "${DEB_DIR}/DEBIAN"
cp "${SCRIPT_DIR}/deb/control" "${DEB_DIR}/DEBIAN/"
chmod 0755 "${DEB_DIR}/DEBIAN"
dpkg-deb --build "${DEB_DIR}"
mv "${BUILD_DIR}/deb.deb" "${BUILD_DIR}/kbh2_v2.x.x_linux_x64.deb"

# create portable AppImage
mkdir -p "${BUILD_DIR}/AppImage/usr/bin"
cp "${BUILD_DIR}/kleiner-brauhelfer-2" "${BUILD_DIR}/AppImage/usr/bin"
mkdir -p "${BUILD_DIR}/AppImage/usr/share/applications"
cp "${SCRIPT_DIR}/AppImage/kleiner-brauhelfer-2.desktop" "${BUILD_DIR}/AppImage/usr/share/applications"
mkdir -p "${BUILD_DIR}/AppImage/usr/share/icons"
cp "${SCRIPT_DIR}/../../kleiner-brauhelfer-2.svg" "${BUILD_DIR}/AppImage/usr/share/icons"
cd "${SCRIPT_DIR}/AppImage"
export QMAKE=${QT_DIR}/qmake
export LD_LIBRARY_PATH=${QT_DIR}/../lib:${QT_DIR}/../../../Tools/QtCreator/lib/Qt/lib
./linuxdeploy-x86_64.AppImage --appdir "${BUILD_DIR}/AppImage" --custom-apprun=./AppRun --plugin qt --output appimage
mv ./kleiner-brauhelfer-2-x86_64.AppImage "${BUILD_DIR}"
