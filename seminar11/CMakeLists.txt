SET(APP_NAME seminar11)

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
	#shaders10/instancingNoMatrix.vert	
	#shaders10/instancingUniform.vert
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_11_1_HorizontalSplit)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders11 DESTINATION ${CMAKE_INSTALL_PREFIX})