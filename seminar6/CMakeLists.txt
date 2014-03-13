SET(APP_NAME seminar6)

SET(SRC_FILES
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Application.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Camera.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/CommonMaterial.cpp	
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Mesh.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Program.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Material.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/SkyBoxMaterial.cpp
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Textures.cpp
)

SET(HEADER_FILES
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Application.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Camera.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/CommonMaterial.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Material.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/Mesh.h
	${DemoApplication_SOURCE_DIR}/${APP_NAME}/SkyBoxMaterial.h
)

ADD_EXECUTABLE(${APP_NAME} ${SRC_FILES} ${HEADER_FILES})

TARGET_LINK_LIBRARIES(${APP_NAME} ${GLEW_LIBRARY} ${GLFW_LIBRARY} ${GLIMAGE_LIBRARY})

IF(WIN32)
	TARGET_LINK_LIBRARIES(${APP_NAME} opengl32.lib)
ELSEIF(WIN32)
	TARGET_LINK_LIBRARIES(${APP_NAME} ${GL_LIBRARY})
ENDIF(WIN32)

INSTALL(TARGETS ${APP_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
