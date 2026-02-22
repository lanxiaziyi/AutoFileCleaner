@echo off
REM Auto File Cleaner Build Script for Windows
REM This script helps you build the Qt project

setlocal enabledelayedexpansion

echo.
echo ========================================
echo   Auto File Cleaner - Build Script
echo ========================================
echo.

REM Check if Qt is installed
where qmake >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: qmake not found in PATH
    echo Please make sure Qt is installed and qmake is in your PATH
    echo.
    pause
    exit /b 1
)

echo Found qmake: %QMAKE_PATH%
echo.

REM Create build directory
if not exist "build" (
    mkdir build
    echo Created build directory
)

cd build

REM Run qmake
echo Running qmake...
qmake ..\AutoFileCleaner.pro -spec win32-msvc

if %errorlevel% neq 0 (
    echo qmake failed
    cd ..
    pause
    exit /b 1
)

echo.
echo ========================================
echo   Choose build type:
echo   1. Debug Mode
echo   2. Release Mode
echo ========================================
echo.
set /p choice="Enter your choice (1 or 2): "

if "%choice%"=="1" (
    echo Building Debug version...
    nmake debug
) else if "%choice%"=="2" (
    echo Building Release version...
    nmake release
) else (
    echo Invalid choice
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ========================================
echo   Build Complete!
echo ========================================
echo.
echo Executable location:
if "%choice%"=="1" (
    echo   build\debug\AutoFileCleaner.exe
) else (
    echo   build\release\AutoFileCleaner.exe
)
echo.
pause
