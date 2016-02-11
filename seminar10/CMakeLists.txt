set(SRC_FILES	
	Application.cpp
	Texture.cpp
	Framebuffer.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.cpp
)

set(HEADER_FILES
	Application.h
	Texture.h
	Framebuffer.h
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.hpp
)

set(SHADER_FILES
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

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_10_1_Instancing)
MAKE_SAMPLE(Sample_10_2_Particles)
MAKE_SAMPLE(Sample_10_3_TransformFeedback)
MAKE_SAMPLE(Sample_10_4_GeometryShader)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar10/shaders10 DESTINATION ${CMAKE_INSTALL_PREFIX})