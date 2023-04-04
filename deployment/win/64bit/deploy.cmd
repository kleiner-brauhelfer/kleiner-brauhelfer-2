@echo off

set BIN_DIR=%1
Set BIN_DIR=%BIN_DIR:"=%
set QT_DIR=%2
Set QT_DIR=%QT_DIR:"=%

del "%BIN_DIR%\kleiner-brauhelfer-core.lib"

xcopy "%~dp0libcrypto-1_1-x64.dll" "%BIN_DIR%" /Y
xcopy "%~dp0libssl-1_1-x64.dll" "%BIN_DIR%" /Y

"%QT_DIR%\windeployqt.exe" --no-translations "%BIN_DIR%\kleiner-brauhelfer-2.exe"
