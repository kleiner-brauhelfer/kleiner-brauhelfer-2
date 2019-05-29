@echo off

set QT_DIR=%1
if "%QT_DIR%" equ "" (
  echo Usage: build_win32.cmd ^<qt-bin-directory^>
  exit /b 1
)

if not exist "%QT_DIR%\qmake.exe" (
  echo Error: qmake.exe not found in ^<qt-bin-directory^>
  exit /b 1
)

call "%PROGRAMFILES(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat" > nul 2>&1

set PRO=%~dp0kleiner-brauhelfer-2.pro
set BUILD_DIR=%~dp0build-win32

mkdir %BUILD_DIR%
cd %BUILD_DIR%
"%QT_DIR%\qmake.exe" "%PRO%" -config release
nmake
"%QT_DIR%\lupdate.exe" "%PRO%"
"%QT_DIR%\lrelease.exe" "%PRO%"
cd %~dp0

"%~dp0deployment/win/32bit/deploy.cmd" "%BUILD_DIR%\bin" "%QT_DIR%"
