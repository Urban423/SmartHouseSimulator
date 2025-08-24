set(ZLIB_ROOT "/opt/zlib-windows")
set(ZLIB_INCLUDE_DIR "${ZLIB_ROOT}/include")
set(ZLIB_LIBRARY "${ZLIB_ROOT}/lib/libz.a") 
include_directories(${ZLIB_INCLUDE_DIR})

set(GLAD_ROOT "/opt/glad-windows")
set(GLAD_INCLUDE_DIR "${GLAD_ROOT}/include")
set(GLAD_LIBRARY "${GLAD_ROOT}/lib/libglad.a") 
include_directories(${GLAD_INCLUDE_DIR})