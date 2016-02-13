macro(MAKE_SAMPLE TRGTNAME)
	add_executable(${TRGTNAME} ${TRGTNAME}.cpp ${SRC_FILES} ${HEADER_FILES} ${SHADER_FILES})
	
	target_include_directories(${TRGTNAME} PUBLIC ${PROJECT_SOURCE_DIR}/common)

	target_link_libraries(${TRGTNAME} GLEW::glew_s glm glfw ASSIMP::assimp SOIL::soil ${ANTTWEAKBAR_LIBRARY})

	install(TARGETS ${TRGTNAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
endmacro(MAKE_SAMPLE TRGTNAME)

macro(MAKE_TASK TRGTNAME)
	add_executable(${TRGTNAME} ${SRC_FILES} ${HEADER_FILES})
	
	target_include_directories(${TRGTNAME} PUBLIC ${PROJECT_SOURCE_DIR}/common)

	target_link_libraries(${TRGTNAME} GLEW::glew_s glm glfw ASSIMP::assimp SOIL::soil ${ANTTWEAKBAR_LIBRARY})

	install(TARGETS ${TRGTNAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
endmacro(MAKE_TASK TRGTNAME)
