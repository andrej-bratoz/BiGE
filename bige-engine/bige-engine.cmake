

function (bige_use_library_rel_deb TGT LIB)
    target_link_libraries(${TGT} PUBLIC
        $<IF:$<CONFIG:Debug>,${LIB}d.lib,${LIB}.lib>
    )
endfunction()

function (bige_use_library TGT LIB)
    target_link_libraries(${TGT} PUBLIC "${LIB}")
endfunction()


function(bige_use_extern_library NAME TGT)
    if(${NAME} STREQUAL "glfw")
        target_include_directories(${TGT} PUBLIC "${BIGECC_DIR}/glfw/include")
        bige_use_library(${TGT} "${BIGECC_DIR}/glfw/lib/glfw3.lib")
    endif()

    if(${NAME} STREQUAL "json")
        target_include_directories(${TGT} PUBLIC "${BIGECC_DIR}/json")
    endif()

    if(${NAME} STREQUAL "glm")
        target_include_directories(${TGT} PUBLIC "${BIGECC_DIR}/glm/include")
        target_link_libraries(${TGT} PUBLIC "${BIGECC_DIR}/glm/lib/glm.lib")
    endif()

    if(${NAME} STREQUAL "glad")
        target_include_directories(${TGT} PUBLIC "${BIGECC_DIR}/glad/include")
    endif()

    if(${NAME} STREQUAL "stb")
        target_include_directories(${TGT} PUBLIC "${BIGECC_DIR}/stb")
    endif()

    if(${NAME} STREQUAL "gltf")
        target_include_directories(${TGT} PUBLIC "${BIGECC_DIR}/tinygltf/include")
        #target_link_libraries(${TGT} PUBLIC "${BIGECC_DIR}/tinygltf/lib/tinygltf.lib")
        bige_use_library_rel_deb(${TGT} "${BIGECC_DIR}/tinygltf/lib/tinygltf")
    endif()
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
        bige_use_extern_library_glfw(${target_name})
        target_compile_definitions(${target_name} PRIVATE "-DBIGE_ENGINE_HAS_EDITOR")
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
    target_include_directories(${TGT} PUBLIC "${BIGE_ENGINE_DIR}")
    target_link_libraries(${TGT} PUBLIC opengl32 ${BINGE_ENGINE_LIB})
    bige_use_extern_library_glfw(${TGT})
    bige_use_extern_library_glad(${TGT})
    bige_use_extern_library_glm(${TGT})
    bige_use_extern_library_json(${TGT})
    bige_use_extern_library_gltf(${TGT})
endfunction()