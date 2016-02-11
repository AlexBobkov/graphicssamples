SET(APP_NAME seminar5)

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
	shaders5/texture.vert
	shaders5/texture.frag
	
	shaders5/textureSpecular.vert
	shaders5/textureSpecular.frag
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_5_1_Texturing)
MAKE_SAMPLE(Sample_5_2_Magnification)
MAKE_SAMPLE(Sample_5_3_Minification)
MAKE_SAMPLE(Sample_5_4_Specular)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders5 DESTINATION ${CMAKE_INSTALL_PREFIX})