macro(MAKE_SAMPLE TRGTNAME)
    add_executable(${TRGTNAME} ${TRGTNAME}.cpp ${SRC_FILES} ${HEADER_FILES} ${SHADER_FILES})

    target_include_directories(${TRGTNAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/common
        ${ASSIMP_INCLUDE_DIR}
    )
    
    if(OCULUS_SDK_LIBRARIES)
        target_include_directories(${TRGTNAME} PUBLIC ${OCULUS_SDK_INCLUDE_DIRS})
    endif()

    target_link_libraries(${TRGTNAME}
        GLEW::glew_s
        glm
        glfw        
        SOIL::soil
        IMGUI::imgui
        ${ASSIMP_LIBRARY}
    )
    
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${TRGTNAME} PRIVATE -std=c++11)
        target_link_libraries(${TRGTNAME} "-framework CoreFoundation")
    endif()

    if(CMAKE_COMPILER_IS_GNUCC)
        target_compile_options(${TRGTNAME} PRIVATE -std=c++11)
    endif()

    install(TARGETS ${TRGTNAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
endmacro(MAKE_SAMPLE TRGTNAME)
