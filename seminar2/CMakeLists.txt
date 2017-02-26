set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/common/Application.cpp
    ${PROJECT_SOURCE_DIR}/common/Camera.cpp
    ${PROJECT_SOURCE_DIR}/common/Mesh.cpp
    ${PROJECT_SOURCE_DIR}/common/ShaderProgram.cpp
)

set(HEADER_FILES
    ${PROJECT_SOURCE_DIR}/common/Application.hpp
    ${PROJECT_SOURCE_DIR}/common/Camera.hpp
    ${PROJECT_SOURCE_DIR}/common/Mesh.hpp
    ${PROJECT_SOURCE_DIR}/common/ShaderProgram.hpp
)

set(SHADER_FILES
    shaders2/shader.vert
    shaders2/shader.frag
    shaders2/shaderNormal.vert
    shaders2/shaderDiscard.frag
    shaders2/shaderLine.vert
    shaders2/shaderPoint.vert
    shaders2/shaderQuad.vert
    shaders2/shaderQuad.frag
    shaders2/shaderQuadMandelbrot.frag
    shaders2/shaderTime.vert
    shaders2/shaderTime.frag
    shaders2/shaderTimeCoord.vert
    shaders2/shaderTimeCoord.frag
    shaders2/shaderUBO.vert
    shaders2/simple.vert
    shaders2/simple.frag
    shaders2/simpleMat.vert
)

source_group("Shaders" FILES
    ${SHADER_FILES}
)

MAKE_SAMPLE(Sample_02_1_Transformation)
MAKE_SAMPLE(Sample_02_2_Shaders)
MAKE_SAMPLE(Sample_02_3_Points)
MAKE_SAMPLE(Sample_02_4_Lines)
MAKE_SAMPLE(Sample_02_5_ScreenSpaceQuad)
MAKE_SAMPLE(Sample_02_6_UBO)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar2/shaders2 DESTINATION ${CMAKE_INSTALL_PREFIX})
