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
mv "${DEPLOY_DIR}/../deb.deb" "./kbh2_v2.x.x_linux_x86.deb"

