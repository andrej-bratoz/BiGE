cmake_minimum_required (VERSION 3.12)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project("Andro_2D")

SET(VERSION "0.1.0")

OPTION(BUILD_SHARED_LIBS "Build shared library" ON)
OPTION(BUILD_TESTS "Build tests" ON)

FILE(GLOB ANB_CORE_INC 
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/include/a2d_controls/*.h"
)
FILE(GLOB ANB_CORE_SRC 
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/a2d_controls/*.cpp"
)
include_directories(${PROJECT_SOURCE_DIR}/include/)
SET(andro2d-src ${ANB_CORE_INC} ${ANB_CORE_SRC})

if(BUILD_SHARED_LIBS)
    add_library(andro2d SHARED ${andro2d-src})
    target_link_libraries(andro2d gdiplus)
    target_compile_definitions(andro2d PRIVATE ANDRO2D_LIB)
else()
    add_library(andro2d STATIC ${andro2d-src})
endif(BUILD_SHARED_LIBS)
set_target_properties(andro2d PROPERTIES CXX_STANDARD 20)


FILE(GLOB SAMPLE_APP_SRC "${PROJECT_SOURCE_DIR}/samples/*.cpp")
add_executable(andro_2d_sample ${SAMPLE_APP_SRC})
target_link_libraries(andro_2d_sample andro2d gdiplus)