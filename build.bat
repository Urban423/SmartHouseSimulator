@echo off

SET IMAGE_NAME=windows_builder
SET PROJECT_PATH=%~dp0
SET PLATFORM=windows

if not "%~1"=="" set PLATFORM=%~1

echo build for %PLATFORM%

docker --version > nul 2>&1
IF ERRORLEVEL 1 ( 
	echo Docker not found. 
	pause
	exit /b
)

docker image inspect %IMAGE_NAME% >nul 2>&1
IF ERRORLEVEL 1 (
	echo IMAGE not found. Building new image.
	docker build -t %IMAGE_NAME% .
) ELSE (
	echo image found
)

echo start Building
docker run --rm -v "%PROJECT_PATH%:/workspace" %IMAGE_NAME% bash -c "cmake --preset=%PLATFORM% && cmake --build .CMakeFiles"