@echo off
SET IMAGE_NAME=windows_builder
SET PROJECT_NAME=project
SET PROJECT_PATH=%~dp0
SET PLATFORM=windows


if "%~1"=="clean" (
    echo Cleaning build environment...

    docker volume rm %PROJECT_NAME% >nul 2>&1
    docker volume create %PROJECT_NAME% >nul 2>&1

    docker builder prune -a -f >nul 2>&1
    exit /b
)


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

docker volume inspect %PROJECT_NAME% >nul 2>&1
IF ERRORLEVEL 1 (
    echo Volume not found.
    docker volume create %PROJECT_NAME%
) ELSE (
    echo Volume found
)

echo Start building
docker run --rm -v "%PROJECT_NAME%:/workspace" -v "%PROJECT_PATH%:/host_project" %IMAGE_NAME% bash /host_project/tools/build.sh %PLATFORM%
@REM start "" "./build/GlassWars/GlassWars.exe"
"./build/GlassWars/GlassWars.exe"