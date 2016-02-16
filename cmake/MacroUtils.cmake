macro(MAKE_SAMPLE TRGTNAME)
	add_executable(${TRGTNAME} ${TRGTNAME}.cpp ${SRC_FILES} ${HEADER_FILES} ${SHADER_FILES})
	
	target_include_directories(${TRGTNAME} PUBLIC ${PROJECT_SOURCE_DIR}/common)

	target_link_libraries(${TRGTNAME} GLEW::glew_s glm glfw ASSIMP::assimp SOIL::soil imgui)
	
	if (CMAKE_COMPILER_IS_GNUCC)
		target_compile_options(${TRGTNAME} PUBLIC -std=c++11)
	endif ()

	install(TARGETS ${TRGTNAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
endmacro(MAKE_SAMPLE TRGTNAME)
