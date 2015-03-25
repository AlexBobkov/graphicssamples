SET(APP_NAME seminar7)

SET(SRC_FILES	
	Application.cpp
	Mesh.cpp
	ShaderProgram.cpp
	Texture.cpp
)

SET(HEADER_FILES
	Application.h
	Mesh.h
	ShaderProgram.h
	Texture.h
)

SET(SHADER_FILES
	shaders7/projector.vert	
	shaders7/projector.frag

	shaders7/quad.vert
	shaders7/quad.frag	
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_7_1_Projector)
MAKE_SAMPLE(Sample_7_2_CopyTex)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders7 DESTINATION ${CMAKE_INSTALL_PREFIX})