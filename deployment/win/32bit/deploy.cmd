@echo off

set BUILD_DIR=%1
Set BUILD_DIR=%BUILD_DIR:"=%
set QT_DIR=%2
Set QT_DIR=%QT_DIR:"=%

del "%BUILD_DIR%\kleiner-brauhelfer-core.lib"

"%QT_DIR%\windeployqt.exe" --no-translations "%BUILD_DIR%\kleiner-brauhelfer-2.exe"
