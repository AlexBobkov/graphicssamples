#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Framebuffer.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <algorithm>

#define NOMINMAX
#include <Windows.h>

#include <OVR_CAPI_0_5_0.h>
#include <OVR_CAPI_GL.h>

struct LightInfo
{
    glm::vec3 position; //Будем здесь хранить координаты в мировой системе координат, а при копировании в юниформ-переменную конвертировать в систему виртуальной камеры
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

float frand()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void getColorFromLinearPalette(float value, float& r, float& g, float& b);

/**
Инстансинг
*/
class SampleApplication : public Application
{
public:
    Mesh cube;
    Mesh sphere;
    Mesh bunny;
    Mesh ground;
    Mesh backgroundCube;

    Mesh teapot;
    Mesh teapotArray;

    Mesh quad;

    Mesh marker; //Меш - маркер для источника света

    //Идентификатор шейдерной программы
    ShaderProgram _commonShader;
    ShaderProgram _markerShader;
    ShaderProgram _skyboxShader;
    ShaderProgram _quadDepthShader;
    ShaderProgram _quadColorShader;
    ShaderProgram _oculusShader;

    //Переменные для управления положением одного источника света
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;
    CameraInfo _lightCamera;

    GLuint _worldTexId;
    GLuint _brickTexId;
    GLuint _grassTexId;
    GLuint _chessTexId;
    GLuint _myTexId;
    GLuint _cubeTexId;

    GLuint _sampler;
    GLuint _cubeTexSampler;
    GLuint _depthSampler;

    float _oldTime;
    float _deltaTime;
    float _fps;
    std::deque<float> _fpsData;
       
    ovrHmd _hmd;
    ovrEyeRenderDesc _eyeRenderDesc[2];
    ovrVector2f UVScaleOffset[2][2];
    ovrRecti viewports[2];

    GLuint _distortionMeshVao[2];
    int _numIndices;

    Framebuffer _oculusFB;
    GLuint _oculusTexId;
    GLuint _depthTexId;

    ~SampleApplication()
    {
        if (_hmd)
        {
            ovrHmd_Destroy(_hmd);
        }

        ovr_Shutdown();
    }

    void initOVR()
    {
        ovr_Initialize();

        _hmd = ovrHmd_Create(0);
        if (!_hmd)
        {
            std::cerr << "Failed to create real hmd. Try to use debug one.\n";

            _hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
        }

        ovrBool result = ovrHmd_ConfigureTracking(_hmd, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0);
        if (!result)
        {
            std::cerr << "Failed to setup tracking\n";
        }

        //--------------------------------------------------
        //Конфигурируем фреймбуфер для рендера исходных изображений для левого и правого глаз
        
        ovrSizei recommenedTex0Size = ovrHmd_GetFovTextureSize(_hmd, ovrEye_Left, _hmd->DefaultEyeFov[0], 1.0f);
        ovrSizei recommenedTex1Size = ovrHmd_GetFovTextureSize(_hmd, ovrEye_Right, _hmd->DefaultEyeFov[1], 1.0f);
        ovrSizei renderTargetSize;
        renderTargetSize.w = recommenedTex0Size.w + recommenedTex1Size.w;
        renderTargetSize.h = std::max(recommenedTex0Size.h, recommenedTex1Size.h);

        _oculusFB.create();
        _oculusFB.bind();
        _oculusFB.setSize(renderTargetSize.w, renderTargetSize.h);

        _oculusTexId = _oculusFB.addBuffer(GL_RGBA, GL_COLOR_ATTACHMENT0);
        _depthTexId = _oculusFB.addBuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT);

        _oculusFB.initDrawBuffers();

        if (!_oculusFB.valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        _oculusFB.unbind();

        //--------------------------------------------------

        viewports[0].Pos.x = 0;
        viewports[0].Pos.y = 0;
        viewports[0].Size.w = _oculusFB.width() / 2;
        viewports[0].Size.h = _oculusFB.height();

        viewports[1].Pos.x = _oculusFB.width() / 2;
        viewports[1].Pos.y = 0;
        viewports[1].Size.w = _oculusFB.width() / 2;
        viewports[1].Size.h = _oculusFB.height();

        //--------------------------------------------------
       
        _eyeRenderDesc[0] = ovrHmd_GetRenderDesc(_hmd, ovrEye_Left, _hmd->DefaultEyeFov[0]);
        _eyeRenderDesc[1] = ovrHmd_GetRenderDesc(_hmd, ovrEye_Right, _hmd->DefaultEyeFov[1]);

        //--------------------------------------------------

        glGenVertexArrays(2, _distortionMeshVao);
        
        //Generate distortion mesh for each eye
        for (int eyeNum = 0; eyeNum < 2; eyeNum++)
        {
            ovrDistortionMesh meshData;
            ovrHmd_CreateDistortionMesh(_hmd,
                                        _eyeRenderDesc[eyeNum].Eye,
                                        _eyeRenderDesc[eyeNum].Fov,
                                        _hmd->DistortionCaps, &meshData);

            ovrHmd_GetRenderScaleAndOffset(_eyeRenderDesc[eyeNum].Fov,
                                           renderTargetSize, viewports[eyeNum],
                                           UVScaleOffset[eyeNum]);

            ovrDistortionVertex* ov = meshData.pVertexData;
            unsigned short* index = meshData.pIndexData;

            _numIndices = meshData.IndexCount;

            std::vector<float> vertices;
            std::vector<float> colors;
            std::vector<float> texRarray;
            std::vector<float> texGarray;
            std::vector<float> texBarray;
            std::vector<unsigned short> indices;

            for (unsigned vertNum = 0; vertNum < meshData.VertexCount; vertNum++)
            {
                vertices.push_back(ov[vertNum].ScreenPosNDC.x);
                vertices.push_back(ov[vertNum].ScreenPosNDC.y);                

                colors.push_back(ov[vertNum].VignetteFactor);
                colors.push_back(ov[vertNum].VignetteFactor);
                colors.push_back(ov[vertNum].VignetteFactor);
                colors.push_back(ov[vertNum].TimeWarpFactor);

                texRarray.push_back(ov[vertNum].TanEyeAnglesR.x);
                texRarray.push_back(ov[vertNum].TanEyeAnglesR.y);

                texGarray.push_back(ov[vertNum].TanEyeAnglesG.x);
                texGarray.push_back(ov[vertNum].TanEyeAnglesG.y);

                texBarray.push_back(ov[vertNum].TanEyeAnglesB.x);
                texBarray.push_back(ov[vertNum].TanEyeAnglesB.y);
            }

            for (unsigned indexNum = 0; indexNum < meshData.IndexCount; ++indexNum)
            {
                indices.push_back(index[indexNum]);
            }

            glBindVertexArray(_distortionMeshVao[eyeNum]);

            GLuint vbo = 0;

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, texRarray.size() * sizeof(float), texRarray.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, texGarray.size() * sizeof(float), texGarray.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, NULL);

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, texBarray.size() * sizeof(float), texBarray.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, NULL);

            GLuint ibo = 0;
            glGenBuffers(1, &ibo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);

            glBindVertexArray(0);

            // Deallocate the mesh data
            ovrHmd_DestroyDistortionMesh(&meshData);
        }        
    }

    virtual void makeScene()
    {
        Application::makeScene();

        initOVR();

        //=========================================================

        _oldTime = 0.0;
        _deltaTime = 0.0;
        _fps = 0.0;

        //=========================================================
        //Создание и загрузка мешей		

        cube.makeCube(0.5);
        cube.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));

        sphere.makeSphere(0.5, 100);
        sphere.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));

        bunny.loadFromFile("models/bunny.obj");
        bunny.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        teapot.loadFromFile("models/teapot.obj");

        ground.makeGroundPlane(5.0f, 2.0f);

        marker.makeSphere(0.1);

        backgroundCube.makeCube(10.0f);

        quad.makeScreenAlignedQuad();

        //=========================================================
        //Инициализация шейдеров

        _commonShader.createProgram("shaders6/common.vert", "shaders6/common.frag");
        _markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");
        _skyboxShader.createProgram("shaders6/skybox.vert", "shaders6/skybox.frag");
        _quadDepthShader.createProgram("shaders7/quadDepth.vert", "shaders7/quadDepth.frag");
        _quadColorShader.createProgram("shaders7/quadColor.vert", "shaders7/quadColor.frag");
        _oculusShader.createProgram("shaders11/oculus.vert", "shaders11/oculus.frag");

        //=========================================================
        //Инициализация значений переменных освщения
        _lr = 10.0;
        _phi = 0.0f;
        _theta = 0.48f;

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _light.ambient = glm::vec3(0.2, 0.2, 0.2);
        _light.diffuse = glm::vec3(0.8, 0.8, 0.8);
        _light.specular = glm::vec3(1.0, 1.0, 1.0);

        //=========================================================
        //Загрузка и создание текстур
        _worldTexId = Texture::loadTexture("images/earth_global.jpg");
        _brickTexId = Texture::loadTexture("images/brick.jpg");
        _grassTexId = Texture::loadTexture("images/grass.jpg");
        _chessTexId = Texture::loadTextureWithMipmaps("images/chess.dds");
        _myTexId = Texture::makeProceduralTexture();
        _cubeTexId = Texture::loadCubeTexture("images/cube");

        //=========================================================
        //Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
        glGenSamplers(1, &_sampler);
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenSamplers(1, &_cubeTexSampler);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        GLfloat border[] = { 1.0f, 0.0f, 0.0f, 1.0f };

        glGenSamplers(1, &_depthSampler);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glSamplerParameterfv(_depthSampler, GL_TEXTURE_BORDER_COLOR, border);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    }

    virtual void initGUI()
    {
        Application::initGUI();

        TwAddVarRO(_bar, "FPS", TW_TYPE_FLOAT, &_fps, "");
        TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
        TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
        TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
        TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_light.ambient, "group=Light label='ambient'");
        TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_light.diffuse, "group=Light label='diffuse'");
        TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_light.specular, "group=Light label='specular'");
    }

    virtual void handleKey(int key, int scancode, int action, int mods)
    {
        Application::handleKey(key, scancode, action, mods);

        if (action == GLFW_PRESS)
        {
            ovrHSWDisplayState hswDisplayState;
            ovrHmd_GetHSWDisplayState(_hmd, &hswDisplayState);
            if (hswDisplayState.Displayed)
            {
                ovrHmd_DismissHSWDisplay(_hmd);
            }

            if (key == GLFW_KEY_SPACE)
            {
            }
        }
    }

    void computeFPS()
    {
        _deltaTime = glfwGetTime() - _oldTime;
        _oldTime = glfwGetTime();
        _fpsData.push_back(1 / _deltaTime);
        while (_fpsData.size() > 10)
        {
            _fpsData.pop_front();
        }

        _fps = 0.0;
        if (_fpsData.size() > 0)
        {
            for (unsigned int i = 0; i < _fpsData.size(); i++)
            {
                _fps += _fpsData[i];
            }
            _fps /= _fpsData.size();
            _fps = floor(_fps);
        }
    }

    void update()
    {
        Application::update();

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _lightCamera.viewMatrix = glm::lookAt(_light.position, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        _lightCamera.projMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 30.f);

        computeFPS();
    }

    virtual void draw()
    {
        ovrFrameTiming frameTiming = ovrHmd_BeginFrameTiming(_hmd, 0);

        _oculusFB.bind();

        glViewport(0, 0, _oculusFB.width(), _oculusFB.height());
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                
        
        ovrVector3f ViewOffset[2] = { _eyeRenderDesc[0].HmdToEyeViewOffset, _eyeRenderDesc[1].HmdToEyeViewOffset };
        ovrPosef EyeRenderPose[2];
        ovrHmd_GetEyePoses(_hmd, 0, ViewOffset, EyeRenderPose, NULL);

        for (int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++)
        {
            CameraInfo camera;

            ovrMatrix4f proj = ovrMatrix4f_Projection(_eyeRenderDesc[eyeIndex].Fov, 0.01f, 10000.0f, true);

            camera.projMatrix = glm::mat4(proj.M[0][0], proj.M[1][0], proj.M[2][0], proj.M[3][0],
                                          proj.M[0][1], proj.M[1][1], proj.M[2][1], proj.M[3][1],
                                          proj.M[0][2], proj.M[1][2], proj.M[2][2], proj.M[3][2],
                                          proj.M[0][3], proj.M[1][3], proj.M[2][3], proj.M[3][3]);

            
            glm::vec3 pos = glm::vec3(EyeRenderPose[eyeIndex].Position.x, EyeRenderPose[eyeIndex].Position.y, EyeRenderPose[eyeIndex].Position.z);
            glm::quat rot = glm::quat(EyeRenderPose[eyeIndex].Orientation.w, EyeRenderPose[eyeIndex].Orientation.x, EyeRenderPose[eyeIndex].Orientation.y, EyeRenderPose[eyeIndex].Orientation.z);

            camera.viewMatrix = glm::translate(glm::mat4(1.0f), -pos) * glm::mat4_cast(glm::inverse(rot)) * _camera.viewMatrix;
            
            glViewport(viewports[eyeIndex].Pos.x, viewports[eyeIndex].Pos.y, viewports[eyeIndex].Size.w, viewports[eyeIndex].Size.h);
            drawScene(_commonShader, camera);
        }

        glBindSampler(0, 0);
        glUseProgram(0);
        _oculusFB.unbind();

        ovr_WaitTillTime(frameTiming.TimewarpPointSeconds);

        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _oculusShader.use();

#if 1
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _oculusTexId);
        glBindSampler(0, _sampler);
        _oculusShader.setIntUniform("Texture", 0);

        for (int eyeIndex = 0; eyeIndex < 2; eyeIndex++)
        {
            _oculusShader.setVec2Uniform("EyeToSourceUVScale", glm::vec2(UVScaleOffset[eyeIndex][0].x, UVScaleOffset[eyeIndex][0].y));
            _oculusShader.setVec2Uniform("EyeToSourceUVOffset", glm::vec2(UVScaleOffset[eyeIndex][1].x, UVScaleOffset[eyeIndex][1].y));
            
            ovrMatrix4f timeWarpMatrices[2];
            ovrHmd_GetEyeTimewarpMatrices(_hmd, (ovrEyeType)eyeIndex,
                                          EyeRenderPose[eyeIndex],
                                          timeWarpMatrices);

            glm::mat4 start = glm::mat4(timeWarpMatrices[0].M[0][0], timeWarpMatrices[0].M[1][0], timeWarpMatrices[0].M[2][0], timeWarpMatrices[0].M[3][0],
                                        timeWarpMatrices[0].M[0][1], timeWarpMatrices[0].M[1][1], timeWarpMatrices[0].M[2][1], timeWarpMatrices[0].M[3][1],
                                        timeWarpMatrices[0].M[0][2], timeWarpMatrices[0].M[1][2], timeWarpMatrices[0].M[2][2], timeWarpMatrices[0].M[3][2],
                                        timeWarpMatrices[0].M[0][3], timeWarpMatrices[0].M[1][3], timeWarpMatrices[0].M[2][3], timeWarpMatrices[0].M[3][3]);

            glm::mat4 end = glm::mat4(timeWarpMatrices[1].M[0][0], timeWarpMatrices[1].M[1][0], timeWarpMatrices[1].M[2][0], timeWarpMatrices[1].M[3][0],
                                      timeWarpMatrices[1].M[0][1], timeWarpMatrices[1].M[1][1], timeWarpMatrices[1].M[2][1], timeWarpMatrices[1].M[3][1],
                                      timeWarpMatrices[1].M[0][2], timeWarpMatrices[1].M[1][2], timeWarpMatrices[1].M[2][2], timeWarpMatrices[1].M[3][2],
                                      timeWarpMatrices[1].M[0][3], timeWarpMatrices[1].M[1][3], timeWarpMatrices[1].M[2][3], timeWarpMatrices[1].M[3][3]);

            _oculusShader.setMat4Uniform("EyeRotationStart", start);
            _oculusShader.setMat4Uniform("EyeRotationEnd", end);

            glBindVertexArray(_distortionMeshVao[eyeIndex]);
            glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_SHORT, 0);
        }
#else        
        _quadColorShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _oculusTexId);
        glBindSampler(0, _sampler);
        _quadColorShader.setIntUniform("tex", 0);

        quad.draw();
#endif

        //Отсоединяем сэмплер и шейдерную программу
        glBindSampler(0, 0);
        glUseProgram(0);
                
        ovrHmd_EndFrameTiming(_hmd);
    }

    void drawScene(const ShaderProgram& shader, const CameraInfo& camera)
    {
        shader.use();

        //Загружаем на видеокарту значения юниформ-переменных
        shader.setMat4Uniform("viewMatrix", camera.viewMatrix);
        shader.setMat4Uniform("projectionMatrix", camera.projMatrix);

        glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_light.position, 1.0));
        shader.setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
        shader.setVec3Uniform("light.La", _light.ambient);
        shader.setVec3Uniform("light.Ld", _light.diffuse);
        shader.setVec3Uniform("light.Ls", _light.specular);

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
        glBindTexture(GL_TEXTURE_2D, _brickTexId);
        glBindSampler(0, _sampler);
        shader.setIntUniform("diffuseTex", 0);

        //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
        {
            shader.setMat4Uniform("modelMatrix", cube.modelMatrix());
            shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * cube.modelMatrix()))));

            cube.draw();
        }

        {
            shader.setMat4Uniform("modelMatrix", sphere.modelMatrix());
            shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * sphere.modelMatrix()))));

            sphere.draw();
        }

        {
            shader.setMat4Uniform("modelMatrix", bunny.modelMatrix());
            shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * bunny.modelMatrix()))));

            bunny.draw();
        }

        {
            shader.setMat4Uniform("modelMatrix", ground.modelMatrix());
            shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * ground.modelMatrix()))));

            ground.draw();
        }
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}