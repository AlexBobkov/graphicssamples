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
	
	shaders4/marker.vert
	shaders4/marker.frag
)

SOURCE_GROUP("Shaders" FILES	
	${SHADER_FILES}	
)

MAKE_SAMPLE(Sample_4_1)
MAKE_SAMPLE(Sample_4_2)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/${APP_NAME}/shaders4 DESTINATION ${CMAKE_INSTALL_PREFIX})