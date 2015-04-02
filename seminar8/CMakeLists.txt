SET(APP_NAME seminar8)

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
	shaders8/toshadow.vert	
	shaders8/toshadow.frag
	
	shaders8/shadow.vert
	shaders8/shadow.frag
	shaders8/shadow2.frag
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_8_1_Shadows)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders8 DESTINATION ${CMAKE_INSTALL_PREFIX})