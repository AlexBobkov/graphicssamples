set(SRC_FILES	
	Application.cpp
	ShaderProgram.cpp
	Texture.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
)

set(HEADER_FILES
	Application.h
	ShaderProgram.h
	Texture.h
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
)

set(SHADER_FILES
	shaders5/texture.vert
	shaders5/texture.frag
	
	shaders5/textureSpecular.vert
	shaders5/textureSpecular.frag
)

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_5_1_Texturing)
MAKE_SAMPLE(Sample_5_2_Magnification)
MAKE_SAMPLE(Sample_5_3_Minification)
MAKE_SAMPLE(Sample_5_4_Specular)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar5/shaders5 DESTINATION ${CMAKE_INSTALL_PREFIX})