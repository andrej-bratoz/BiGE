

function (bige_use_library_rel_deb TGT LIB)
    if(WIN32)
        target_link_libraries(${TGT} PRIVATE
            $<IF:$<CONFIG:Debug>,${LIB}d.lib,${LIB}.lib>
        )
    else()
         target_link_libraries(${TGT} PRIVATE "lib${LIB}.a")
    endif()
endfunction()

function (bige_use_library TGT LIB)
    target_link_libraries(${TGT} PRIVATE "${LIB}")
endfunction()


function(bige_create_gui_wrapper)
    if(TARGET "bige_imgui_wrapper") 
        return()
    endif()
    add_library("bige_imgui_wrapper" STATIC 
        "${BIGECC_DIR}/imgui/imgui.cpp"
        "${BIGECC_DIR}/imgui/imgui_draw.cpp"
        "${BIGECC_DIR}/imgui/imgui_tables.cpp"
        "${BIGECC_DIR}/imgui/imgui_widgets.cpp"
        "${BIGECC_DIR}/imgui/backends/imgui_impl_glfw.cpp"
        "${BIGECC_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
    )
    target_include_directories(bige_imgui_wrapper PRIVATE "${BIGECC_DIR}/imgui")
    target_include_directories(bige_imgui_wrapper PRIVATE "${BIGECC_DIR}/imgui/backends")
    add_dependencies("bige_imgui_wrapper" ${EXTERN_LIBS_PROJECT_NAME})
    bige_use_extern_library_glfw("bige_imgui_wrapper")

endfunction()

function(bige_use_extern_library NAME TGT)
    if(${NAME} STREQUAL "editor")
        bige_create_gui_wrapper()
        target_link_libraries(${TGT} PRIVATE "bige_imgui_wrapper")
        target_include_directories(${TGT} PRIVATE "${BIGECC_DIR}/imgui")
        target_include_directories(${TGT} PRIVATE "${BIGECC_DIR}/imgui/backends")
    endif()

    if(${NAME} STREQUAL "glfw")
        target_include_directories(${TGT} PUBLIC "${BIGECC_DIR}/glfw/include")
        bige_use_library_rel_deb(${TGT} "${BIGECC_DIR}/glfw/lib/glfw3")
    endif()

    if(${NAME} STREQUAL "json")
        target_include_directories(${TGT} PUBLIC "${BIGECC_DIR}/nlohmann-json/include")
    endif()

    if(${NAME} STREQUAL "glm")
        target_include_directories(${TGT} PUBLIC "${BIGECC_DIR}/glm/include")
        bige_use_library_rel_deb(${TGT} "${BIGECC_DIR}/glm/lib/glm")
    endif()

    if(${NAME} STREQUAL "glad")
        target_include_directories(${TGT} PUBLIC "${BIGECC_DIR}/glad/include")
    endif()

    if(${NAME} STREQUAL "stb")
        target_include_directories(${TGT} PUBLIC "${BIGECC_DIR}/stb")
    endif()

    if(${NAME} STREQUAL "gltf")
        target_include_directories(${TGT} PUBLIC "${BIGECC_DIR}/tinygltf/include")
        bige_use_library_rel_deb(${TGT} "${BIGECC_DIR}/tinygltf/lib/tinygltf")
    endif()
endfunction()

function(bige_use_extern_library_editor TGT) 
    bige_use_extern_library("editor" ${TGT})
endfunction()

function(bige_use_extern_library_glfw TGT) 
    bige_use_extern_library("glfw" ${TGT})
endfunction()

function(bige_use_extern_library_json TGT) 
    bige_use_extern_library("json" ${TGT})
endfunction()

function(bige_use_extern_library_glm TGT) 
    bige_use_extern_library("glm" ${TGT})
endfunction()

function(bige_use_extern_library_stb TGT) 
    bige_use_extern_library("stb" ${TGT})
endfunction()


function(bige_use_extern_library_gltf TGT) 
    bige_use_extern_library("gltf" ${TGT})
endfunction()

function(bige_use_extern_library_glad TGT) 
    bige_use_extern_library("glad" ${TGT})
endfunction()


function(create_bige_engine OPTIONS_LIST TARGET_OUT)
    set(target_name "bige_engine")
    set(target_files "")

    if(${OPTIONS_LIST} MATCHES "editor")
        set(has_editor 1)
        string(APPEND target_name "_we_")
    endif()

    if(${OPTIONS_LIST} MATCHES "glfw")
        set(has_glfw 1)
        string(APPEND target_name "_glfw_")
    endif()

    if(${OPTIONS_LIST} MATCHES "glm")
        set(has_glm 1)
        string(APPEND target_name "_glm_")
    endif()

    if(${OPTIONS_LIST} MATCHES "glad")
        set(has_glad 1)
        string(APPEND target_name "_glad_")
        list(APPEND target_files "${BIGECC_DIR}/glad/src/glad.c")
    endif()

    if(${OPTIONS_LIST} MATCHES "json")
        set(has_json 1)
        string(APPEND target_name "_json_")
    endif()

    if(${OPTIONS_LIST} MATCHES "stb")
        set(has_stb 1)
        string(APPEND target_name "_stb_")
    endif()

    if(${OPTIONS_LIST} MATCHES "gltf")
        set(has_gltf 1)
        string(APPEND target_name "_gltf_")
        #list(APPEND target_files "${BIGECC_DIR}/gltf/tiny_gltf.cpp")
    endif()

    file(GLOB engine_header_files "${BIGE_ENGINE_DIR}/*h")
    file(GLOB engine_source_files "${BIGE_ENGINE_DIR}/*cpp")

    file(GLOB engine_materials_header_files "${BIGE_ENGINE_DIR}/materials/*h")
    file(GLOB engine_materials_source_files "${BIGE_ENGINE_DIR}/materials/*cpp")
    
    list(APPEND target_files ${engine_header_files})
    list(APPEND target_files ${engine_source_files})

    list(APPEND target_files ${engine_materials_header_files})
    list(APPEND target_files ${engine_materials_source_files})

    # check if the target already exists
    if(TARGET "${target_name}") 
        set(${TARGET_OUT} "${target_name}" PARENT_SCOPE)
        return()
    endif()

    add_library(${target_name} STATIC)
    set(${TARGET_OUT} "${target_name}" PARENT_SCOPE)
    target_include_directories(${target_name} PUBLIC "${BIGE_ENGINE_DIR}")

    
    if(has_editor EQUAL 1)
        bige_use_extern_library_editor(${target_name})
        file(GLOB editor_files_h "${BIGE_EDITOR_DIR}/we-*.h")
        file(GLOB editor_files_cpp "${BIGE_EDITOR_DIR}/we-*.cpp")
        list(APPEND target_files "${editor_files_h}" ${editor_files_cpp})

        target_compile_definitions(${target_name} PRIVATE "-DBIGE_ENGINE_HAS_EDITOR")
        target_include_directories(${target_name} PRIVATE ${BIGE_EDITOR_DIR})
    endif()

    if(has_glfw EQUAL 1)
        bige_use_extern_library_glfw(${target_name})
        target_compile_definitions(${target_name} PRIVATE "-DBIGE_ENGINE_HAS_GLFW")
    endif()

    if(has_glad EQUAL 1)
        bige_use_extern_library_glad(${target_name})
        target_compile_definitions(${target_name} PRIVATE "-DBIGE_ENGINE_HAS_GLAD")
    endif()

    if(has_json EQUAL 1)
        bige_use_extern_library_json(${target_name})
        target_compile_definitions(${target_name} PRIVATE "-DBIGE_ENGINE_HAS_JSON")
    endif()

    if(has_stb EQUAL 1)
        bige_use_extern_library_stb(${target_name})
        target_compile_definitions(${target_name} PRIVATE "-DBIGE_ENGINE_HAS_STB")
    endif()

    if(has_gltf EQUAL 1)
        bige_use_extern_library_gltf(${target_name})
        target_compile_definitions(${target_name} PRIVATE "-DBIGE_ENGINE_HAS_GLTF")
    endif()

     if(has_glm EQUAL 1)
        bige_use_extern_library_glm(${target_name})
        target_compile_definitions(${target_name} PRIVATE "-DBIGE_ENGINE_HAS_GLM")
    endif()

    target_sources(${target_name} PRIVATE ${target_files})

endfunction()

function(use_bige_engine OPTIONS_LIST TGT)
    create_bige_engine(${OPTIONS_LIST} BINGE_ENGINE_LIB)
    if(MSVC)
        target_compile_options("${BINGE_ENGINE_LIB}" PRIVATE /permissive)
    endif()
    target_include_directories(${TGT} PUBLIC "${BIGE_ENGINE_DIR}")
    target_link_libraries(${TGT} PUBLIC opengl32 ${BINGE_ENGINE_LIB})
    bige_use_extern_library_glfw(${TGT})
    bige_use_extern_library_glad(${TGT})
    bige_use_extern_library_glm(${TGT})
    bige_use_extern_library_json(${TGT})
    bige_use_extern_library_gltf(${TGT})
    add_dependencies(${TGT} ${EXTERN_LIBS_PROJECT_NAME})
endfunction()