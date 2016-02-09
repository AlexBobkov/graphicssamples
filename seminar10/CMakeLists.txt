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
	shaders10/instancingUniform.vert	
	shaders10/instancingTexture.vert	
	shaders10/instancingDivisor.vert
	
	shaders10/particle.vert	
	shaders10/particle.frag
	
	shaders10/transformFeedbackPass1.vert
	shaders10/transformFeedbackPass2.vert
	
	shaders10/particleWithGeomShader.vert	
	shaders10/particleWithGeomShader.geom
	shaders10/particleWithGeomShader.frag
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_10_1_Instancing)
MAKE_SAMPLE(Sample_10_2_Particles)
MAKE_SAMPLE(Sample_10_3_TransformFeedback)
MAKE_SAMPLE(Sample_10_4_GeometryShader)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders10 DESTINATION ${CMAKE_INSTALL_PREFIX})