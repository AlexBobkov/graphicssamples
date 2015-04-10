SET(APP_NAME seminar9)

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
	shaders9/deferred.vert	
	shaders9/deferred.frag
	
	shaders9/quad.vert
	
	shaders9/grayscale.frag
	shaders9/gamma.frag
	
	shaders9/bright.frag
	shaders9/horizblur.frag
	shaders9/vertblur.frag
	shaders9/tonemapping.frag
	
	shaders9/ssao.frag
	shaders9/deferredWithSSAO.frag
	
	shaders9/dof.frag
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_9_1_Grayscale)
MAKE_SAMPLE(Sample_9_2_Gamma)
MAKE_SAMPLE(Sample_9_3_HDR)
MAKE_SAMPLE(Sample_9_4_SSAO)
MAKE_SAMPLE(Sample_9_5_DoF)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders9 DESTINATION ${CMAKE_INSTALL_PREFIX})