if(CMAKE_FIND_ROOT_PATH)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif()


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

set(OPENGL_LIBRARIES opengl32)
set(OPENGL_DIR "")

list(APPEND LINK_LIBS gdi32 opengl32 ${ZLIB_LIBRARY} ${GLAD_LIBRARY})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
add_compile_definitions(glad)
add_compile_definitions(windowsOS)



math(EXPR NUM_LIBRARIES "${NUM_LIBRARIES} - 1")
foreach(INDEX RANGE ${NUM_LIBRARIES})
	set(LIB "${LIBRARY_${INDEX}}")
	create_library(${LIB} src/${LIB})
	message(STATUS "lib: ${LINK_LIBS}")
	list(APPEND LINK_LIBS ${LIB})
endforeach()


add_executable(${CMAKE_PROGRAM_NAME} src/${PATH_TO_MAIN})
target_link_libraries(${CMAKE_PROGRAM_NAME} ${LINK_LIBS})