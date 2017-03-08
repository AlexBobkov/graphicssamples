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
    shaders3/diffuseDirectionalLight.vert
    shaders3/diffuseDirectionalLight.frag

    shaders3/diffusePointLight.vert
    shaders3/diffusePointLight.frag

    shaders3/specularPointLightPerVertex.vert
    shaders3/specularPointLightPerVertex.frag

    shaders3/specularPointLightPerFragment.vert
    shaders3/specularPointLightPerFragment.frag

    shaders3/specularAttenuationPointLightPerFragment.vert
    shaders3/specularAttenuationPointLightPerFragment.frag

    shaders3/CookTorrance.vert
    shaders3/CookTorrance.frag

    shaders3/manyLights.vert
    shaders3/manyLights.frag
    
    shaders3/lightingUBO.vert
    shaders3/lightingUBO.frag
)

source_group("Shaders" FILES
    ${SHADER_FILES}
)

MAKE_SAMPLE(Sample_03_1_DirectionalLight)
MAKE_SAMPLE(Sample_03_2_PointLight)
MAKE_SAMPLE(Sample_03_3_PerFragment)
MAKE_SAMPLE(Sample_03_4_Attenuation)
MAKE_SAMPLE(Sample_03_5_CookTorrance)
MAKE_SAMPLE(Sample_03_6_MultiLights)
MAKE_SAMPLE(Sample_03_7_LightingUBO)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/seminar3/shaders3 DESTINATION ${CMAKE_INSTALL_PREFIX})