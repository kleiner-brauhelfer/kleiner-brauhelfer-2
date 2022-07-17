#!/bin/sh

BUILD_DIR=$1
QT_DIR=$2
SCRIPT_DIR=$(dirname "$(readlink -f "${0}")")

# remove unnecessary stuff
rm "${BUILD_DIR}/libkleiner-brauhelfer-core.a"


# create deb package 
DEPLOY_DIR="${BUILD_DIR}/deb"

mkdir -p "${DEPLOY_DIR}/usr/local/bin"
cp "${BUILD_DIR}/kleiner-brauhelfer-2" "${DEPLOY_DIR}/usr/local/bin"

mkdir -p "${DEPLOY_DIR}/usr/share/pixmaps"
cp "${SCRIPT_DIR}/../../kleiner-brauhelfer-2.svg" "${DEPLOY_DIR}/usr/share/pixmaps/"

mkdir -p "${DEPLOY_DIR}/usr/share/applications"
cp "${SCRIPT_DIR}/deb/kleiner-brauhelfer-2.desktop" "${DEPLOY_DIR}/usr/share/applications/"

mkdir -p "${DEPLOY_DIR}/DEBIAN"
cp "${SCRIPT_DIR}/deb/control" "${DEPLOY_DIR}/DEBIAN/"

chmod 0755 "${DEPLOY_DIR}/DEBIAN"
dpkg-deb --build "${DEPLOY_DIR}"
mv "${DEPLOY_DIR}/../deb.deb" "./kbh2_v2.x.x_linux_x64.deb"


# create portable AppImage
mkdir -p "${BUILD_DIR}/AppImage/usr/bin"
cp "${BUILD_DIR}/kleiner-brauhelfer-2" "${BUILD_DIR}/AppImage/usr/bin"
cp "${SCRIPT_DIR}/AppImage/AppRun" "${BUILD_DIR}/AppImage"

mkdir -p "${BUILD_DIR}/AppImage/usr/share/applications"
cp "${SCRIPT_DIR}/AppImage/kleiner-brauhelfer-2.desktop" "${BUILD_DIR}/AppImage/usr/share/applications"

mkdir -p "${BUILD_DIR}/AppImage/usr/share/icons"
cp "${SCRIPT_DIR}/../../kleiner-brauhelfer-2.svg" "${BUILD_DIR}/AppImage/usr/share/icons"

cd "${BUILD_DIR}/AppImage"
ARCH=x86_64 "${SCRIPT_DIR}/AppImage/linuxdeployqt-6-x86_64.AppImage" "./usr/share/applications/kleiner-brauhelfer-2.desktop" -qmake="${QT_DIR}/qmake" -no-copy-copyright-files -no-translations -appimage -unsupported-allow-new-glibc -extra-plugins=platforms/
cd -
mv ${BUILD_DIR}/AppImage/kleiner-brauhelfer-2-*-x86_64.AppImage ./kleiner-brauhelfer-2-x86_64.AppImage
