set(SRC_FILES	
	${PROJECT_SOURCE_DIR}/common/Application.cpp
	${PROJECT_SOURCE_DIR}/common/Camera.cpp
	${PROJECT_SOURCE_DIR}/common/Framebuffer.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.cpp
	${PROJECT_SOURCE_DIR}/common/Texture.cpp
)

set(HEADER_FILES
	${PROJECT_SOURCE_DIR}/common/Application.hpp
	${PROJECT_SOURCE_DIR}/common/Camera.hpp
	${PROJECT_SOURCE_DIR}/common/Framebuffer.hpp
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.hpp
	${PROJECT_SOURCE_DIR}/common/Texture.hpp
)

set(SHADER_FILES
	shaders8/deferred.frag

	shaders8/grayscale.frag
    shaders8/sepia.frag
	shaders8/gamma.frag

	shaders8/bright.frag
	shaders8/horizblur.frag
	shaders8/vertblur.frag
	shaders8/tonemapping.frag

	shaders8/ssao.frag
	shaders8/deferredWithSSAO.frag

	shaders8/dof.frag
)

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_08_1_Grayscale)
MAKE_SAMPLE(Sample_08_2_Gamma)
MAKE_SAMPLE(Sample_08_3_HDR)
MAKE_SAMPLE(Sample_08_4_SSAO)
MAKE_SAMPLE(Sample_08_5_DoF)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar8/shaders8 DESTINATION ${CMAKE_INSTALL_PREFIX})