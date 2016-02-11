set(SRC_FILES	
	Application.cpp
	ShaderProgram.cpp
	Texture.cpp
	Framebuffer.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
)

set(HEADER_FILES
	Application.h
	ShaderProgram.h
	Texture.h
	Framebuffer.h
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
)

set(SHADER_FILES
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

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_9_1_Grayscale)
MAKE_SAMPLE(Sample_9_2_Gamma)
MAKE_SAMPLE(Sample_9_3_HDR)
MAKE_SAMPLE(Sample_9_4_SSAO)
MAKE_SAMPLE(Sample_9_5_DoF)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar9/shaders9 DESTINATION ${CMAKE_INSTALL_PREFIX})