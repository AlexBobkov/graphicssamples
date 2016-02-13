#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

#include <iostream>
#include <sstream>
#include <vector>

struct LightInfo
{
    glm::vec3 position; //����� ����� ������� ���������� � ������� ������� ���������, � ��� ����������� � �������-���������� �������������� � ������� ����������� ������
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

/**
2 ����������� ������ � 2 ��������
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;
    MeshPtr _backgroundCube;

    MeshPtr _marker; //��� - ������ ��� ��������� �����

    //������������� ��������� ���������
    ShaderProgram _commonShader;
    ShaderProgram _markerShader;
    ShaderProgram _skyboxShader;

    //���������� ��� ���������� ���������� ������ ��������� �����
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;

    GLuint _brickTexId;
    GLuint _cubeTexId;

    GLuint _sampler;
    GLuint _cubeTexSampler;

    CameraInfo _camera2;

    void makeScene() override
    {
        Application::makeScene();

        //=========================================================
        //�������� � �������� �����		

        _cube = makeCube(0.5f);
        _cube->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f)));

        _sphere = makeSphere(0.5f);
        _sphere->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f)));

        _bunny = loadFromFile("models/bunny.obj");
        _bunny->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        _marker = makeSphere(0.1f);

        _backgroundCube = makeCube(10.0f);

        //=========================================================
        //������������� ��������

        _commonShader.createProgram("shaders6/common.vert", "shaders6/common.frag");
        _markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");
        _skyboxShader.createProgram("shaders6/skybox.vert", "shaders6/skybox.frag");

        //=========================================================
        //������������� �������� ���������� ��������
        _lr = 10.0;
        _phi = 2.65f;
        _theta = 0.48f;

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _light.ambient = glm::vec3(0.2, 0.2, 0.2);
        _light.diffuse = glm::vec3(0.8, 0.8, 0.8);
        _light.specular = glm::vec3(1.0, 1.0, 1.0);

        //=========================================================
        //�������� � �������� �������
        _brickTexId = Texture::loadTexture("images/brick.jpg");
        _cubeTexId = Texture::loadCubeTexture("images/cube");

        //=========================================================
        //������������� ��������, �������, ������� ������ ��������� ������ �� ��������
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

        //=========================================================
        //������������� 2� ����������� ������

        _camera2.viewMatrix = glm::lookAt(glm::vec3(-5.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f));

        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        _camera2.projMatrix = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.f);
    }

    void initGUI() override
    {
        Application::initGUI();

        TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
        TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
        TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
        TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_light.ambient, "group=Light label='ambient'");
        TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_light.diffuse, "group=Light label='diffuse'");
        TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_light.specular, "group=Light label='specular'");
    }

    void draw() override
    {
        //�������� ������� ������� ������ � ��������� �������
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //������� ������ ����� � ������� �� ����������� ���������� ����������� �����
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, width / 2, height / 2);

        drawSceneWithCamera(_camera);

        glViewport(width / 2, height / 2, width / 2, height / 2);

        drawSceneWithCamera(_camera2);
    }

    void drawSceneWithCamera(const CameraInfo& camera)
    {
        //====== ������ ��� � ���������� ��������� ======
        {
            _skyboxShader.use();

            glm::vec3 cameraPos = glm::vec3(glm::inverse(camera.viewMatrix)[3]); //��������� �� ������� ���� ��������� ����������� ������ � ������� ������� ���������

            _skyboxShader.setVec3Uniform("cameraPos", cameraPos);
            _skyboxShader.setMat4Uniform("viewMatrix", camera.viewMatrix);
            _skyboxShader.setMat4Uniform("projectionMatrix", camera.projMatrix);

            glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
            glBindTexture(GL_TEXTURE_CUBE_MAP, _cubeTexId);
            glBindSampler(0, _cubeTexSampler);
            _skyboxShader.setIntUniform("cubeTex", 0);

            glDepthMask(GL_FALSE); //��������� ������ � ����� �������

            _backgroundCube->draw();

            glDepthMask(GL_TRUE); //�������� ������� ������ � ����� �������
        }

        //====== ������ �������� ������� ����� ======
        _commonShader.use();

        //��������� �� ���������� �������� �������-����������
        _commonShader.setMat4Uniform("viewMatrix", camera.viewMatrix);
        _commonShader.setMat4Uniform("projectionMatrix", camera.projMatrix);

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_light.position, 1.0));

        _commonShader.setVec3Uniform("light.pos", lightPosCamSpace); //�������� ��������� ��� � ������� ����������� ������
        _commonShader.setVec3Uniform("light.La", _light.ambient);
        _commonShader.setVec3Uniform("light.Ld", _light.diffuse);
        _commonShader.setVec3Uniform("light.Ls", _light.specular);

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
        glBindTexture(GL_TEXTURE_2D, _brickTexId);
        glBindSampler(0, _sampler);
        _commonShader.setIntUniform("diffuseTex", 0);

        //��������� �� ���������� ������� ������ ����� � ��������� ���������
        {
            _commonShader.setMat4Uniform("modelMatrix", _cube->modelMatrix());
            _commonShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _cube->modelMatrix()))));

            _cube->draw();
        }

        {
            _commonShader.setMat4Uniform("modelMatrix", _sphere->modelMatrix());
            _commonShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _sphere->modelMatrix()))));

            _sphere->draw();
        }

        {
            _commonShader.setMat4Uniform("modelMatrix", _bunny->modelMatrix());
            _commonShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _bunny->modelMatrix()))));

            _bunny->draw();
        }

        //������ ������� ��� ���� ���������� �����		
        {
            _markerShader.use();

            _markerShader.setMat4Uniform("mvpMatrix", camera.projMatrix * camera.viewMatrix * glm::translate(glm::mat4(1.0f), _light.position));
            _markerShader.setVec4Uniform("color", glm::vec4(_light.diffuse, 1.0f));
            _marker->draw();
        }

        //����������� ������� � ��������� ���������
        glBindSampler(0, 0);
        glUseProgram(0);
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}