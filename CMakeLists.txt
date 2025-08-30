cmake_minimum_required(VERSION 3.14)
project(${CMAKE_PROJECT_NAME} VERSION 1.0)

include(cmake/directoryManager.cmake)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows") 
	include(cmake/windows.cmake)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
	include(cmake/android.cmake)
endif()

include(cmake/common.cmake)
include(cmake/assetsManager.cmake)
