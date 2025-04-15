cmake_minimum_required(VERSION 3.14)
project(${CMAKE_PROJECT_NAME} VERSION 1.0)




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
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}")
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



function(create_library LIB_NAME LIB_PATH)
	file(GLOB_RECURSE SOURCES "${LIB_PATH}/*.cpp")
    file(GLOB_RECURSE HEADER_DIRS LIST_DIRECTORIES true "${LIB_PATH}/*")
	
	set(FILTERED_SOURCES "")
	foreach(source ${SOURCES})
		set(EXCLUDE FALSE)
		foreach(excluded ${EXCLUDED_DIRS})
			if ("${source}" MATCHES ".*/${excluded}/.*")
				set(EXCLUDE TRUE)
			endif()
		endforeach()
		if (NOT EXCLUDE)
			list(APPEND FILTERED_SOURCES ${source})
		endif()
	endforeach()
	
	set(INCLUDE_DIRS "")
    foreach(dir ${HEADER_DIRS})
        if (IS_DIRECTORY ${dir})
            list(APPEND INCLUDE_DIRS ${dir})
        endif()
    endforeach()
	
	
	
	add_library(${LIB_NAME} STATIC ${FILTERED_SOURCES})
	target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/${LIB_PATH}" ${INCLUDE_DIRS})
	target_link_libraries(${LIB_NAME} PUBLIC ${LINK_LIBS})
endfunction()

function(include_library LIB_NAME)
	
endfunction()

setFolderName()





find_package(OpenGL REQUIRED)
add_library(glad STATIC IMPORTED)
set_target_properties(glad PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/libs/libglad.a")
set_target_properties(glad PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/libs/include")
list(APPEND LINK_LIBS glad gdi32 opengl32 z)
add_compile_definitions(glad)
add_compile_definitions(windows)






math(EXPR NUM_LIBRARIES "${NUM_LIBRARIES} - 1")
foreach(INDEX RANGE ${NUM_LIBRARIES})
	set(LIB "${LIBRARY_${INDEX}}")
	create_library(${LIB} ${LIB})
	message(STATUS "lib: ${LINK_LIBS}")
	list(APPEND LINK_LIBS ${LIB})
endforeach()



add_executable(${CMAKE_PROGRAM_NAME} ${PATH_TO_MAIN})
target_link_libraries(${CMAKE_PROGRAM_NAME} ${LINK_LIBS})


add_custom_target(run ALL
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROGRAM_NAME} ${PROGRAM_INPUT_DATA}
    DEPENDS ${CMAKE_PROJECT_NAME}
    COMMENT "Running the program after build"
)