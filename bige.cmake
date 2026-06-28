include("${BIGE_EDITOR_DIR}/engine-editor.cmake")
include("${BIGE_ENGINE_DIR}/bige-engine.cmake")

set(GAME_NAME "BinkoAdventure")
set(GAME_FOLDER "badv")
set(TESTS_NAME "BiGETests")
set(WORLD_EDITOR_NAME "BiGEWorldEditor")

file(GLOB SOURCE_FILES_MAIN "${BIGE_GAME_DIR}/badv/*cpp")
file(GLOB SOURCE_FILES_MAIN2 "${BIGE_GAME_DIR}/badv/*h")
add_executable(${GAME_NAME} ${SOURCE_FILES_MAIN} ${SOURCE_FILES_MAIN2})
use_bige_engine("glfw glad stb gltf glm" ${GAME_NAME})
set_target_properties(${GAME_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)

#set(INC_CATCH2 "${BIGE_ENGINE_ROOT}/external/Catch2/include")
#set(LIB_CATCH2_DIR "${BIGE_ENGINE_ROOT}/external/Catch2/lib")
#add_executable()


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
            COMMENT "Copying ${REL_PATH} to ${DEST_DIRNAME}"
        )
    endforeach()
endmacro()
copy_folder_to_bin( ${GAME_NAME} "${BIGE_GAME_DIR}/${GAME_FOLDER}/config" "config")
copy_folder_to_bin( ${GAME_NAME} "${BIGE_GAME_DIR}/${GAME_FOLDER}/assets" "assets")
copy_folder_to_bin( ${GAME_NAME} "${BIGE_ENGINE_DIR}/shaders" "shaders")

