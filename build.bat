@echo off
cmake --preset=default
attrib +H .CMakeFiles
cmake --build .CMakeFiles
