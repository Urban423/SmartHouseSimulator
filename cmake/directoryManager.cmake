set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/lib")

function(setFolderName)
	set(BUILD_COUNT_FILE "${CMAKE_BINARY_DIR}/build_count.txt")
	if(EXISTS ${BUILD_COUNT_FILE})
		file(READ ${BUILD_COUNT_FILE} CURRENT_BUILD_NUMBER)
		string(STRIP ${CURRENT_BUILD_NUMBER} CURRENT_BUILD_NUMBER)
		math(EXPR NEW_BUILD_NUMBER "${CURRENT_BUILD_NUMBER} + 1")
	else()
		set(NEW_BUILD_NUMBER 1)
	endif()
	file(WRITE ${BUILD_COUNT_FILE} "${NEW_BUILD_NUMBER}")
	
	if(NOT ${ENABLE_VERSIONING_BUILD})
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY	"${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}" CACHE PATH "Directory for runtime output" FORCE)
		file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
		message(STATUS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        return()
	endif()
	
    set(BASE_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}_${PROGRAM_VERSION}.")
    file(GLOB EXISTING_DIRS "${BASE_OUTPUT_DIR}*")
    set(MAX_VERSION 0)
    
    foreach(DIR ${EXISTING_DIRS})
        get_filename_component(DIR_NAME ${DIR} NAME)
        string(REGEX MATCH "[0-9]+$" VERSION_NUMBER ${DIR_NAME})  # Extract number at the end
		message(STATUS "Using output directory: ${DIR_NAME}")

        if(VERSION_NUMBER)
            math(EXPR NUM_VERSION "${VERSION_NUMBER} + 0")  # Convert to number
            if(NUM_VERSION GREATER MAX_VERSION)
                set(MAX_VERSION ${NUM_VERSION})
            endif()
        endif()
    endforeach()

    math(EXPR OLD_VERSION "${MAX_VERSION}")
    set(EXISTING_FOLDER  "${BASE_OUTPUT_DIR}${OLD_VERSION}")
    set(FINAL_OUTPUT_DIR "${BASE_OUTPUT_DIR}${NEW_BUILD_NUMBER}")
	
    if(NOT ${MAX_VERSION} EQUAL 0) 
        file(RENAME ${EXISTING_FOLDER} ${FINAL_OUTPUT_DIR})
    else()
        file(MAKE_DIRECTORY ${FINAL_OUTPUT_DIR})
    endif()
	
	
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${FINAL_OUTPUT_DIR}" CACHE PATH "Directory for runtime output" FORCE)
endfunction()
