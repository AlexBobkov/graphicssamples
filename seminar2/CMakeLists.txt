set(SRC_FILES	
	Application.cpp
	ShaderProgram.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
)

set(HEADER_FILES
	Application.h
	ShaderProgram.h
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
)

set(SHADER_FILES
	shaders2/shader.vert
	shaders2/shader.frag
)

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_2_1_Transformation)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar2/shaders2 DESTINATION ${CMAKE_INSTALL_PREFIX})
