SET(APP_NAME seminar6)

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
	shaders6/common.vert	
	shaders6/common.frag
	
	shaders6/skybox.vert
	shaders6/skybox.frag
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_6_1)
MAKE_SAMPLE(Sample_6_2)
MAKE_SAMPLE(Sample_6_3)
MAKE_SAMPLE(Sample_6_4)
MAKE_SAMPLE(Sample_6_5)
MAKE_SAMPLE(Sample_6_6)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders6 DESTINATION ${CMAKE_INSTALL_PREFIX})