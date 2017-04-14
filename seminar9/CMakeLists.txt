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
    ${PROJECT_SOURCE_DIR}/common/Mesh.hpp
    ${PROJECT_SOURCE_DIR}/common/ShaderProgram.hpp
    ${PROJECT_SOURCE_DIR}/common/Texture.hpp
)

set(SHADER_FILES
    shaders9/instancingNoMatrix.vert
    shaders9/instancingUniform.vert
    shaders9/instancingUBO.vert
    shaders9/instancingSSBO.vert
    shaders9/instancingTexture.vert
    shaders9/instancingDivisor.vert

    shaders9/particle.vert
    shaders9/particle.frag
    
    shaders9/ground.vert
    shaders9/ground.frag

    shaders9/transformFeedbackPass1.vert
    shaders9/transformFeedbackPass2.vert

    shaders9/particleWithGeomShader.vert
    shaders9/particleWithGeomShader.geom
    shaders9/particleWithGeomShader.frag
)

source_group("Shaders" FILES
    ${SHADER_FILES}
)

MAKE_SAMPLE(Sample_09_1_Instancing)
MAKE_SAMPLE(Sample_09_2_Particles)
MAKE_SAMPLE(Sample_09_3_TransformFeedback)
MAKE_SAMPLE(Sample_09_4_GeometryShader)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar9/shaders9 DESTINATION ${CMAKE_INSTALL_PREFIX})