SET(APP_NAME seminar4)

SET(SRC_FILES	
	Application.cpp
	Mesh.cpp
	ShaderProgram.cpp
)

SET(HEADER_FILES
	Application.h
	Mesh.h
	ShaderProgram.h
)

SET(SHADER_FILES
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

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_4_1_DirectionalLight)
MAKE_SAMPLE(Sample_4_2_PointLight)
MAKE_SAMPLE(Sample_4_3_PerFragment)
MAKE_SAMPLE(Sample_4_4_Attenuation)
MAKE_SAMPLE(Sample_4_5_CookTorrance)
MAKE_SAMPLE(Sample_4_6_MultiLights)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders4 DESTINATION ${CMAKE_INSTALL_PREFIX})