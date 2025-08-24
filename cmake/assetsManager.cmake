set(TEXTURE_DIR "${CMAKE_SOURCE_DIR}/Assets")
set(RESOURCE_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets")
file(MAKE_DIRECTORY ${RESOURCE_DIR})
file(GLOB_RECURSE ALL_ASSETS RELATIVE ${TEXTURE_DIR} "${TEXTURE_DIR}/*")
set(RESOURCE_OUTPUTS "")
foreach(file ${ALL_ASSETS})
    set(SRC_FILE "${TEXTURE_DIR}/${file}")
    set(DST_FILE "${RESOURCE_DIR}/${file}")

    get_filename_component(DST_FOLDER ${DST_FILE} PATH)
    file(MAKE_DIRECTORY ${DST_FOLDER})

    add_custom_command(
        OUTPUT ${DST_FILE}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SRC_FILE} ${DST_FILE}
        DEPENDS ${SRC_FILE}
        COMMENT "Copying ${file}"
    )
    list(APPEND RESOURCE_OUTPUTS ${DST_FILE})
endforeach()


add_custom_target(resources ALL
    DEPENDS ${RESOURCE_OUTPUTS}
    COMMENT "Copying all resources"
)