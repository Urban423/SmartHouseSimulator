@echo off
SET IMAGE_NAME=windows_builder
SET PROJECT_PATH=%~dp0
SET PLATFORM=windows
if not "%~1"=="" set PLATFORM=%~1

echo Build for %PLATFORM%

docker --version >nul 2>&1
IF ERRORLEVEL 1 (
    echo Docker not found.
    pause
    exit /b
)

docker image inspect %IMAGE_NAME% >nul 2>&1
IF ERRORLEVEL 1 (
    echo Image not found. Building new image.
    docker build -t %IMAGE_NAME% .
) ELSE (
    echo Image found
)

echo Start building
docker run --rm -v "%PROJECT_PATH%:/workspace" %IMAGE_NAME% python3 /workspace/tools/build.py %PLATFORM%