set(SRC_FILES	
	${PROJECT_SOURCE_DIR}/common/Application.cpp
	${PROJECT_SOURCE_DIR}/common/Camera.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.cpp
	${PROJECT_SOURCE_DIR}/common/Texture.cpp
)

set(HEADER_FILES
	${PROJECT_SOURCE_DIR}/common/Application.hpp
	${PROJECT_SOURCE_DIR}/common/Camera.hpp
	${PROJECT_SOURCE_DIR}/common/LightInfo.hpp
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
	${PROJECT_SOURCE_DIR}/common/ShaderProgram.hpp
	${PROJECT_SOURCE_DIR}/common/Texture.hpp
)

set(SHADER_FILES
	shaders4/texture.vert
	shaders4/texture.frag
    
    shaders4/textureLookup.vert
	shaders4/textureLookup.frag
	
	shaders4/textureSpecular.vert
	shaders4/textureSpecular.frag
)

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_04_1_Texturing)
MAKE_SAMPLE(Sample_04_2_Magnification)
MAKE_SAMPLE(Sample_04_3_Minification)
MAKE_SAMPLE(Sample_04_4_Specular)
MAKE_SAMPLE(Sample_04_5_LookupTexture)
MAKE_SAMPLE(Sample_04_6_CubeTex)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar4/shaders4 DESTINATION ${CMAKE_INSTALL_PREFIX})