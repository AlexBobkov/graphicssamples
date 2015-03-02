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
	shaders4/colored.vert
	shaders4/colored.frag
	
	shaders4/diffusePerFramentPoint.vert
	shaders4/diffusePerFramentPoint.frag
	
	shaders4/diffusePerVertex.vert
	shaders4/diffusePerVertex.frag
	
	shaders4/diffusePerVertexPoint.vert
	shaders4/diffusePerVertexPoint.frag
	
	shaders4/specular.vert
	shaders4/specular.frag
	
	shaders4/specularBlinn.vert
	shaders4/specularBlinn.frag
	
	shaders4/specularBlinnAttenuation.vert
	shaders4/specularBlinnAttenuation.frag
	
	shaders4/specularBlinnMaterial.vert
	shaders4/specularBlinnMaterial.vert
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_4_1)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders4 DESTINATION ${CMAKE_INSTALL_PREFIX})