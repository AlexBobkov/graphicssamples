set(SRC_FILES	
	Application.cpp
	ShaderProgram.cpp
	${PROJECT_SOURCE_DIR}/common/Mesh.cpp
)

set(HEADER_FILES
	Application.h
	ShaderProgram.h
	${PROJECT_SOURCE_DIR}/common/Mesh.hpp
)

set(SHADER_FILES
	shaders4/diffuseDirectionalLight.vert
	shaders4/diffuseDirectionalLight.frag
	
	shaders4/diffusePointLight.vert
	shaders4/diffusePointLight.frag
	
	shaders4/specularPointLightPerVertex.vert
	shaders4/specularPointLightPerVertex.frag
	
	shaders4/specularPointLightPerFragment.vert	
	shaders4/specularPointLightPerFragment.frag
	
	shaders4/specularAttenuationPointLightPerFragment.vert
	shaders4/specularAttenuationPointLightPerFragment.frag
	
	shaders4/CookTorrance.vert	
	shaders4/CookTorrance.frag
	
	shaders4/manyLights.vert
	shaders4/manyLights.frag
	
	shaders4/marker.vert
	shaders4/marker.frag
)

source_group("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_4_1_DirectionalLight)
MAKE_SAMPLE(Sample_4_2_PointLight)
MAKE_SAMPLE(Sample_4_3_PerFragment)
MAKE_SAMPLE(Sample_4_4_Attenuation)
MAKE_SAMPLE(Sample_4_5_CookTorrance)
MAKE_SAMPLE(Sample_4_6_MultiLights)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar4/shaders4 DESTINATION ${CMAKE_INSTALL_PREFIX})