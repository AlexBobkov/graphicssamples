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

MAKE_SAMPLE(Sample_6_1_CubeTex)
MAKE_SAMPLE(Sample_6_2_Viewports)
MAKE_SAMPLE(Sample_6_3_ZFighting)
MAKE_SAMPLE(Sample_6_4_BackFaceCulling)
MAKE_SAMPLE(Sample_6_5_Transparency)
MAKE_SAMPLE(Sample_6_6_Stencil)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders6 DESTINATION ${CMAKE_INSTALL_PREFIX})