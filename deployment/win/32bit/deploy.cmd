@echo off

set BUILD_DIR=%1
Set BUILD_DIR=%BUILD_DIR:"=%
set QT_DIR=%2
Set QT_DIR=%QT_DIR:"=%

del "%BUILD_DIR%\kleiner-brauhelfer-core.lib"
xcopy "%~dp0libeay32.dll" "%BUILD_DIR%" /Y
xcopy "%~dp0ssleay32.dll" "%BUILD_DIR%" /Y

"%QT_DIR%\windeployqt.exe" "%BUILD_DIR%\kleiner-brauhelfer.exe"

echo. 2>"%BUILD_DIR%\portable"
