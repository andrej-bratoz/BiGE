macro(copy_folder_to_bin2 TARGET SOURCE_DIR DEST_DIRNAME)
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


macro(build_engine_editor NAME)
    add_executable(${NAME} "${BIGE_EDITOR_DIR}/main.cpp" "${BIGE_EDITOR_DIR}/editor-scene.h" "${BIGE_EDITOR_DIR}/editor-scene.cpp")
    set_target_properties(${NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    use_bige_engine("glfw glad stb gltf glm json editor" ${NAME})

    copy_folder_to_bin2( ${NAME} "${BIGE_GAME_DIR}/${GAME_FOLDER}/config" "config")
    copy_folder_to_bin2( ${NAME} "${BIGE_GAME_DIR}/${GAME_FOLDER}/assets" "assets")
    copy_folder_to_bin2( ${NAME} "${BIGE_ENGINE_DIR}/shaders" "shaders")

endmacro()