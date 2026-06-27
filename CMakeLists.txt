cmake_minimum_required(VERSION 3.25)
project("binko-game-engine")
include("engine-editor/engine-editor.cmake")
include("engine/bige-engine.cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(GAME_NAME "BinkoAdventure")
set(Tests "BiGETests")
set(WORLD_EDITOR "BiGEWorldEditor")
set(BIGE_ENGINE_ROOT "${CMAKE_SOURCE_DIR}")

set(INC_CATCH2 "${CMAKE_SOURCE_DIR}/external/Catch2/include")
set(LIB_CATCH2_DIR "${CMAKE_SOURCE_DIR}/external/Catch2/lib")


file(GLOB SOURCE_FILES_MAIN "${CMAKE_SOURCE_DIR}/games/badv/*cpp")
file(GLOB SOURCE_FILES_MAIN2 "${CMAKE_SOURCE_DIR}/games/badv/*h")
add_executable(${GAME_NAME} ${SOURCE_FILES_MAIN} ${SOURCE_FILES_MAIN2})

#target_compile_definitions(${GAME_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
#target_compile_definitions(${GAME_NAME} PRIVATE WINDOWS_LEAN_AND_MEAN)

#set_target_properties(${GAME_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
#target_link_libraries(${GAME_NAME} PUBLIC ${ENGINE_LIBRARY_NAME} opengl32 ${LIB_GLFW} ${LIB_GLM})

use_bige_engine("glfw glad json stb gltf glm" ${GAME_NAME})
set_target_properties(${GAME_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)

macro(copy_folder_to_bin TARGET SOURCE_DIR DEST_DIRNAME)
    # Resolve absolute path (works with relative or absolute SOURCE_DIR)
    get_filename_component(ABS_SOURCE "${SOURCE_DIR}" ABSOLUTE)

    # Collect all files recursively from the source folder
    file(GLOB_RECURSE FOLDER_FILES "${ABS_SOURCE}/*")

    foreach(FILE_PATH IN LISTS FOLDER_FILES)
        # Compute the relative path of each file within SOURCE_DIR
        file(RELATIVE_PATH REL_PATH "${ABS_SOURCE}" "${FILE_PATH}")

        # Add a custom command to copy each file at build time
        add_custom_command(
            TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${FILE_PATH}"
                "$<TARGET_FILE_DIR:${TARGET}>/${DEST_DIRNAME}/${REL_PATH}"
            COMMENT "Copying ${REL_PATH} to bin/${DEST_DIRNAME}"
        )
    endforeach()
endmacro()

#add_custom_command(
#    TARGET ${GAME_NAME} POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy 
#	${CMAKE_SOURCE_DIR}/external/glfw/lib/glfw3.dll
#	$<TARGET_FILE_DIR:${GAME_NAME}>
#)

copy_folder_to_bin( ${GAME_NAME} "${BIGE_ENGINE_ROOT}/games/badv/config" "config")
copy_folder_to_bin( ${GAME_NAME} "${BIGE_ENGINE_ROOT}/games/badv/assets" "assets")
copy_folder_to_bin( ${GAME_NAME} "${BIGE_ENGINE_ROOT}/engine/shaders" "shaders")

#  
#add_custom_command(
#    TARGET ${Tests} POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy 
#	${CMAKE_SOURCE_DIR}/external/glfw/lib/glfw3.dll
#	$<TARGET_FILE_DIR:${Tests}>
#)

