cmake_minimum_required(VERSION 3.25) 
project("BiGE")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

include(ProcessorCount)
ProcessorCount(NPROC)
if(NPROC EQUAL 0)
    set(NPROC 4)   # fallback
endif()
include(ExternalProject)

SET(EXTERNAL_DEPENDENCY_ROOT_DIR "${CMAKE_BINARY_DIR}/.bigecc")
SET(EXTERN_LIBS_PROJECT_NAME bige_extern_libs)

function (bige_copy_ext_project PROJ_NAME)
    file(COPY ${CMAKE_SOURCE_DIR}/ext/${PROJ_NAME} DESTINATION "${EXTERNAL_DEPENDENCY_ROOT_DIR}")
endfunction()

function(bige_build_ext_project PROJ_NAME)
    set(INSTALL_DIR "${EXTERNAL_DEPENDENCY_ROOT_DIR}/${PROJ_NAME}")

    ExternalProject_Add(extern_${PROJ_NAME}
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/ext/${PROJ_NAME}
        PREFIX ${CMAKE_BINARY_DIR}/${PROJ_NAME}
        INSTALL_DIR ${INSTALL_DIR}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_DEBUG_POSTFIX=d ${ARGN}
        DOWNLOAD_COMMAND ""
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --parallel ${NPROC}
    )
    # Make the install dir available to the parent scope for target setup below
    set(${PROJ_NAME}_INSTALL_DIR ${INSTALL_DIR} PARENT_SCOPE)
endfunction()

# --- Step 2: helper to wrap a built submodule as a linkable IMPORTED target ---
function(bige_add_imported_target PROJ_NAME)
    set(options)
    set(oneValueArgs LIB_NAME TYPE)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "" ${ARGN})

    if(NOT ARG_TYPE)
        set(ARG_TYPE STATIC)
    endif()
    if(NOT ARG_LIB_NAME)
        set(ARG_LIB_NAME ${PROJ_NAME})
    endif()

    set(INSTALL_DIR ${${PROJ_NAME}_INSTALL_DIR})
    set(INCLUDE_DIR ${INSTALL_DIR}/include)
    file(MAKE_DIRECTORY ${INCLUDE_DIR})

    add_library(${PROJ_NAME} ${ARG_TYPE} IMPORTED GLOBAL)

    if(ARG_TYPE STREQUAL "STATIC")
        set(LIB_FILE "${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${ARG_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    else()
        set(LIB_FILE "${INSTALL_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}${ARG_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")
    endif()

    set_target_properties(${PROJ_NAME} PROPERTIES
        IMPORTED_LOCATION ${LIB_FILE}
        INTERFACE_INCLUDE_DIRECTORIES ${INCLUDE_DIR}
    )
    add_dependencies(${PROJ_NAME} extern_${PROJ_NAME})
endfunction()

# --- Step 3: header-only variant (glm, nlohmann-json -> no lib file to link) ---
function(bige_add_header_only_target PROJ_NAME)
    set(INSTALL_DIR ${${PROJ_NAME}_INSTALL_DIR})
    set(INCLUDE_DIR ${INSTALL_DIR}/include)
    file(MAKE_DIRECTORY ${INCLUDE_DIR})

    add_library(${PROJ_NAME} INTERFACE IMPORTED GLOBAL)
    set_target_properties(${PROJ_NAME} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${INCLUDE_DIR}
    )
    add_dependencies(${PROJ_NAME} extern_${PROJ_NAME})
endfunction()

# --- Copy source only submodules ---
bige_copy_ext_project(stb)
bige_copy_ext_project(imgui)
bige_copy_ext_project(glad)

# --- Build all submodules ---
bige_build_ext_project(Catch2 -DCATCH_ENABLE_REPRODUCIBLE_BUILD=OFF)
bige_build_ext_project(glfw)
bige_build_ext_project(glm)
bige_build_ext_project(tinygltf)
bige_build_ext_project(nlohmann-json -DJSON_BuildTests=OFF -DJSON_MultipleHeaders=OFF)

# --- Wrap each as an imported target ---
bige_add_imported_target(glfw)   # glfw installs libglfw3.a
bige_add_header_only_target(glm)
bige_add_header_only_target(nlohmann-json)
bige_add_header_only_target(stb)
bige_add_header_only_target(imgui)
bige_add_header_only_target(glad)

set(BIGE_ROOT_DIR   "${CMAKE_SOURCE_DIR}")
set(BIGECC_DIR      "${EXTERNAL_DEPENDENCY_ROOT_DIR}")
set(BIGE_ENGINE_DIR "${CMAKE_SOURCE_DIR}/bige-engine")
set(BIGE_EDITOR_DIR "${CMAKE_SOURCE_DIR}/bige-editor")
set(BIGE_TESTS_DIR  "${CMAKE_SOURCE_DIR}/bige-tests")
set(BIGE_GAME_DIR "${CMAKE_SOURCE_DIR}/bige-games")

add_custom_target(${EXTERN_LIBS_PROJECT_NAME} ALL)
add_dependencies(${EXTERN_LIBS_PROJECT_NAME} extern_Catch2 extern_glfw extern_glm extern_nlohmann-json extern_tinygltf)

include("${CMAKE_SOURCE_DIR}/bige.cmake")