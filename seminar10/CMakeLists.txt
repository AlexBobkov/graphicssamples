SET(APP_NAME seminar10)
SET(SHADERS_FOLDER shaders10)

SET(SRC_FILES
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Application.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Camera.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Light.cpp	
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Mesh.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Navigation.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Program.cpp	
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Texture.cpp
)

SET(HEADER_FILES
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Application.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Camera.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Light.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Mesh.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Texture.h
)

SET(MATERIAL_FILES
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Material.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/ColorMaterial.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/CommonMaterial.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/ScreenAlignedQuadMaterial.h	
	
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Material.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/ColorMaterial.cpp	
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/CommonMaterial.cpp	
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/ScreenAlignedQuadMaterial.cpp	
)

SOURCE_GROUP("Materials" FILES
	${MATERIAL_FILES}	
)

IF(GLIMAGE_FOUND AND ASSIMP_FOUND AND ANTTWEAKBAR_FOUND)

	ADD_EXECUTABLE(${APP_NAME} ${SRC_FILES} ${HEADER_FILES} ${MATERIAL_FILES})

	TARGET_LINK_LIBRARIES(${APP_NAME} ${GLEW_LIBRARY} ${GLFW_LIBRARY} ${GLIMAGE_LIBRARY} ${ASSIMP_LIBRARY} ${ANTTWEAKBAR_LIBRARY})

	IF(WIN32)
		TARGET_LINK_LIBRARIES(${APP_NAME} opengl32.lib)
	ELSE(WIN32)
		TARGET_LINK_LIBRARIES(${APP_NAME} ${OPENGL_LIBRARIES} ${X11_Xxf86vm_LIB} ${X11_Xi_LIB} ${X11_Xrandr_LIB} ${PTHREAD_LIBRARY})
	ENDIF(WIN32)

	INSTALL(TARGETS ${APP_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})

ENDIF(GLIMAGE_FOUND AND ASSIMP_FOUND AND ANTTWEAKBAR_FOUND)