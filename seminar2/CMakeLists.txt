SET(APP_NAME seminar2)

SET(SRC_FILES	
	Application.cpp
	Mesh.cpp
	ShaderProgram.cpp
)

SET(HEADER_FILES
	Application.h
	Mesh.h
	ShaderProgram.h
)

SET(SHADER_FILES
	shaders2/shader.vert
	shaders2/shader.frag
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_2_1)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders2 DESTINATION ${CMAKE_INSTALL_PREFIX})
