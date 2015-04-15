SET(APP_NAME seminar10)

SET(SRC_FILES	
	Application.cpp
	Mesh.cpp
	ShaderProgram.cpp
	Texture.cpp
	Framebuffer.cpp
)

SET(HEADER_FILES
	Application.h
	Mesh.h
	ShaderProgram.h
	Texture.h
	Framebuffer.h
)

SET(SHADER_FILES
	shaders10/instancingNoMatrix.vert	
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_10_1_Instancing)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders10 DESTINATION ${CMAKE_INSTALL_PREFIX})