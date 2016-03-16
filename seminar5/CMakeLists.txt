set(SRC_FILES	
	${PROJECT_SOURCE_DIR}/common/Application.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.cpp
	${PROJECT_SOURCE_DIR}/common/Texture.cpp
)

set(HEADER_FILES
	${PROJECT_SOURCE_DIR}/common/Application.hpp
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.hpp
	${PROJECT_SOURCE_DIR}/common/Texture.hpp
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

MAKE_SAMPLE(Sample_05_1_Texturing)
MAKE_SAMPLE(Sample_05_2_Magnification)
MAKE_SAMPLE(Sample_05_3_Minification)
MAKE_SAMPLE(Sample_05_4_Specular)
MAKE_SAMPLE(Sample_05_5_LookupTexture)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar5/shaders5 DESTINATION ${CMAKE_INSTALL_PREFIX})