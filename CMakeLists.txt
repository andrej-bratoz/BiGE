cmake_minimum_required(VERSION 3.25)
project("binko-game-engine")
include("engine-editor/engine-editor.cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(ENGINE_LIBRARY_NAME "BiGE")
set(GAME_NAME "BinkoAdventure")
set(Tests "BiGETests")
set(GameName "badv")
set(WORLD_EDITOR "BiGEWorldEditor")

file(GLOB HEADER_FILES_BIGE "${CMAKE_SOURCE_DIR}/engine/*h")
file(GLOB SOURCE_FILES_BIGE "${CMAKE_SOURCE_DIR}/engine/*cpp")
file(GLOB HEADER_FILES_BIGE_MATERIALS "${CMAKE_SOURCE_DIR}/engine/materials/*h")
file(GLOB SOURCE_FILES_BIGE_MATERIALS "${CMAKE_SOURCE_DIR}/engine/materials/*cpp")



file(GLOB VERTEX_SHADERS "${CMAKE_SOURCE_DIR}/engine/shaders/vertex/*vs")
file(GLOB FRAGMENT_SHADER "${CMAKE_SOURCE_DIR}/engine/shaders/fragment/*fs")

set(LIB_GLFW "${CMAKE_SOURCE_DIR}/external/glfw/lib/glfw3dll.lib")
set(INC_GLFW "${CMAKE_SOURCE_DIR}/external/glfw/include")
set(LIB_GLAD "${CMAKE_SOURCE_DIR}/external/glad/src/glad.c")
set(INC_GLAD "${CMAKE_SOURCE_DIR}/external/glad/include")
set(INC_JSON "${CMAKE_SOURCE_DIR}/external/json")
set(INC_GLM "${CMAKE_SOURCE_DIR}/external/glm/include")
set(LIB_GLM "${CMAKE_SOURCE_DIR}/external/glm/lib/glm.lib")
set(INC_STB "${CMAKE_SOURCE_DIR}/external/stb")
set(INC_GLTF "${CMAKE_SOURCE_DIR}/external/gltf")
set(LIB_GLTF "${CMAKE_SOURCE_DIR}/external/gltf/tiny_gltf.cpp")

set(INC_CATCH2 "${CMAKE_SOURCE_DIR}/external/Catch2/include")
set(LIB_CATCH2_DIR "${CMAKE_SOURCE_DIR}/external/Catch2/lib")

add_library(${ENGINE_LIBRARY_NAME} STATIC "${HEADER_FILES_BIGE}" "${SOURCE_FILES_BIGE}" "${HEADER_FILES_BIGE_OBJECTS}" 
            "${SOURCE_FILES_BIGE_OBJECTS}" "${LIB_GLAD}" "${LIB_GLTF}" "${HEADER_FILES_BIGE_MATERIALS}" ${SOURCE_FILES_BIGE_MATERIALS})

target_include_directories(${ENGINE_LIBRARY_NAME} PUBLIC ${INC_GLFW})
target_include_directories(${ENGINE_LIBRARY_NAME} PUBLIC ${INC_GLAD})
target_include_directories(${ENGINE_LIBRARY_NAME} PUBLIC ${INC_JSON})
target_include_directories(${ENGINE_LIBRARY_NAME} PUBLIC ${INC_GLM})
target_include_directories(${ENGINE_LIBRARY_NAME} PUBLIC ${INC_STB})
target_include_directories(${ENGINE_LIBRARY_NAME} PUBLIC ${INC_GLTF})
target_include_directories(${ENGINE_LIBRARY_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/engine")
target_link_libraries(${ENGINE_LIBRARY_NAME} PUBLIC opengl32 ${LIB_GLFW} ${LIB_GLM})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/engine PREFIX "Header Files" FILES ${HEADER_FILES_BIGE} ${HEADER_FILES_BIGE_MATERIALS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/engine PREFIX "Source Files" FILES ${SOURCE_FILES_BIGE} ${SOURCE_FILES_BIGE_MATERIALS})


file(GLOB SOURCE_FILES_MAIN "${CMAKE_SOURCE_DIR}/games/${GameName}/*cpp")
file(GLOB SOURCE_FILES_MAIN2 "${CMAKE_SOURCE_DIR}/games/${GameName}/*h")
add_executable(${GAME_NAME} ${SOURCE_FILES_MAIN} ${SOURCE_FILES_MAIN2})
set_target_properties(${GAME_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
target_link_libraries(${GAME_NAME} PUBLIC ${ENGINE_LIBRARY_NAME} opengl32 ${LIB_GLFW} ${LIB_GLM})

file(GLOB SOURCE_FILES_TESTS_MAIN "${CMAKE_SOURCE_DIR}/engine-tests/*cpp")
add_executable(${Tests} "${SOURCE_FILES_TESTS_MAIN}")
target_include_directories(${Tests} PRIVATE "${INC_CATCH2}")
target_link_libraries(${Tests} PRIVATE ${ENGINE_LIBRARY_NAME})
target_link_libraries(${Tests} PRIVATE $<$<CONFIG:Debug>:${LIB_CATCH2_DIR}/Catch2d.lib> $<$<NOT:$<CONFIG:Debug>>:${LIB_CATCH2_DIR}/Catch2.lib>)


build_engine_editor(${WORLD_EDITOR} ${CMAKE_SOURCE_DIR} ${ENGINE_LIBRARY_NAME})

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

add_custom_command(
    TARGET ${GAME_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy 
	${CMAKE_SOURCE_DIR}/external/glfw/lib/glfw3.dll
	$<TARGET_FILE_DIR:${GAME_NAME}>
)

copy_folder_to_bin( ${GAME_NAME} "${CMAKE_SOURCE_DIR}/games/badv/config" "config")
copy_folder_to_bin( ${GAME_NAME} "${CMAKE_SOURCE_DIR}/games/badv/assets" "assets")
copy_folder_to_bin( ${GAME_NAME} "${CMAKE_SOURCE_DIR}/engine/shaders" "shaders")

#  
add_custom_command(
    TARGET ${Tests} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy 
	${CMAKE_SOURCE_DIR}/external/glfw/lib/glfw3.dll
	$<TARGET_FILE_DIR:${Tests}>
)
