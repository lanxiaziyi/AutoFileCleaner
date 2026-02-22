@echo off
REM Run the compiled Auto File Cleaner application

setlocal enabledelayedexpansion

echo.
echo ========================================
echo   Auto File Cleaner - Run Script
echo ========================================
echo.

REM Check if either debug or release executable exists
if exist "build\release\AutoFileCleaner.exe" (
    echo Found Release version...
    call build\release\AutoFileCleaner.exe
    exit /b 0
)

if exist "build\debug\AutoFileCleaner.exe" (
    echo Found Debug version...
    call build\debug\AutoFileCleaner.exe
    exit /b 0
)

echo Error: Could not find compiled executable
echo Please run build.bat first to compile the project
echo.
pause
exit /b 1
