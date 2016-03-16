#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

struct LightInfo
{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float a0;
    float a1;
    float a2;
};

struct MaterialInfo
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

/**
������� � ��������� ���������� ��������� � Uniform Buffer Object
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;

    MeshPtr _marker; //������ ��� ��������� �����

    ShaderProgramPtr _shader;
    ShaderProgramPtr _markerShader;

    //���������� ��������� �����
    float _lr;
    float _phi;
    float _theta;

    //��������� ��������� �����
    LightInfo _light;
    
    //��������� ����������
    MaterialInfo _bunnyMaterial;
    MaterialInfo _cubeMaterial;
    MaterialInfo _sphereMaterial;

    //������ �������-���������� (Uniform Buffer Object)
    GLuint _matricesUbo;
    GLuint _lightUbo;
    GLuint _bunnyMaterialUbo;
    GLuint _cubeMaterialUbo;
    GLuint _sphereMaterialUbo;

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

        //=========================================================
        //������������� ��������

        _shader = std::make_shared<ShaderProgram>();
        _shader->createProgram("shaders4/lightingUBO.vert", "shaders4/lightingUBO.frag");

        _markerShader = std::make_shared<ShaderProgram>();
        _markerShader->createProgram("shaders/marker.vert", "shaders/marker.frag");

        //=========================================================
        //������������� �������� ���������� ��������
        _lr = 5.0;
        _phi = 0.0;
        _theta = glm::pi<float>() * 0.25f;

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _light.ambient = glm::vec3(0.2, 0.2, 0.2);
        _light.diffuse = glm::vec3(0.8, 0.8, 0.8);
        _light.specular = glm::vec3(1.0, 1.0, 1.0);
        _light.a0 = 1.0f;
        _light.a1 = 0.0f;
        _light.a2 = 0.0f;

        //������������� ����������
        _bunnyMaterial.ambient = glm::vec3(1.0, 1.0, 0.0);
        _bunnyMaterial.diffuse = glm::vec3(1.0, 1.0, 0.0);
        _bunnyMaterial.specular = glm::vec3(1.0, 1.0, 1.0);
        _bunnyMaterial.shininess = 128.0f;

        _cubeMaterial.ambient = glm::vec3(0.0, 1.0, 0.0);
        _cubeMaterial.diffuse = glm::vec3(0.0, 1.0, 0.0);
        _cubeMaterial.specular = glm::vec3(1.0, 1.0, 1.0);
        _cubeMaterial.shininess = 128.0f;

        _sphereMaterial.ambient = glm::vec3(1.0, 1.0, 1.0);
        _sphereMaterial.diffuse = glm::vec3(1.0, 1.0, 1.0);
        _sphereMaterial.specular = glm::vec3(1.0, 1.0, 1.0);
        _sphereMaterial.shininess = 128.0f;

        //=========================================================
        //������������� Uniform Buffer Object

        //������� ������� UBO ��� ������ �������� � ����
        //��������� ������� ����������� � ������ ������ ��� �����������, �� ����� ��������� ������ ������� � ������� sizeof(_camera)

        glGenBuffers(1, &_matricesUbo);
        glBindBuffer(GL_UNIFORM_BUFFER, _matricesUbo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(_camera), 0, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        //����������� � 0� ����� ��������
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _matricesUbo);

        //--------------------------

        //������� UBO ��� ��������� �����
        //���������� ��������� ����� ������������� � ������ � ������������,
        //������� ������� ����� ��������� ������ ������ � ������� ������� glGetActiveUniformBlockiv

        GLuint lightBlockIndex = glGetUniformBlockIndex(_shader->id(), "LightInfo");

        GLint lightBlockSize;
        glGetActiveUniformBlockiv(_shader->id(), lightBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &lightBlockSize);
        
        glGenBuffers(1, &_lightUbo);
        glBindBuffer(GL_UNIFORM_BUFFER, _lightUbo);
        glBufferData(GL_UNIFORM_BUFFER, lightBlockSize, 0, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        //����������� � 1� ����� ��������
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, _lightUbo);

        //--------------------------

        //������� UBO ��� ���������
        //���������� ��������� ������������� � ������ � ������������,
        //������� ������� ����� ��������� ������ ������ � ������� ������� glGetActiveUniformBlockiv

        GLuint materialBlockIndex = glGetUniformBlockIndex(_shader->id(), "MaterialInfo");

        GLint materialBlockSize;
        glGetActiveUniformBlockiv(_shader->id(), materialBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &materialBlockSize);

        glGenBuffers(1, &_bunnyMaterialUbo);
        glBindBuffer(GL_UNIFORM_BUFFER, _bunnyMaterialUbo);
        glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, 0, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glGenBuffers(1, &_cubeMaterialUbo);
        glBindBuffer(GL_UNIFORM_BUFFER, _cubeMaterialUbo);
        glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, 0, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glGenBuffers(1, &_sphereMaterialUbo);
        glBindBuffer(GL_UNIFORM_BUFFER, _sphereMaterialUbo);
        glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, 0, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        //��� ��������� ���� �� ��������� ����� ��������, �.�. ���������� ����� ���������, � ����� �� ������

        //����� �������� ��������� ����� ����������. ����� ����� ��������� ����� ������� ����� �����
        //��� ����� ����� �������� ������ ���������� ������ ������

        //����� ���������� ���������
        const char* names[4] =
        {            
            "MaterialInfo.Ka",
            "MaterialInfo.Kd",
            "MaterialInfo.Ks",
            "MaterialInfo.shininess"
        };

        GLuint index[4];
        GLint offset[4];

        //����������� ������� 4� �������-����������
        glGetUniformIndices(_shader->id(), 4, names, index);

        //���� �������, ����������� ������ ��� 4� �������-����������
        glGetActiveUniformsiv(_shader->id(), 4, index, GL_UNIFORM_OFFSET, offset);

        //������� ����� � ����������� ������
        std::vector<GLubyte> buffer;
        buffer.resize(materialBlockSize);

        //��������� ����� �������, ���� ������
        memcpy(buffer.data() + offset[0], &_bunnyMaterial.ambient, sizeof(_bunnyMaterial.ambient));
        memcpy(buffer.data() + offset[1], &_bunnyMaterial.diffuse, sizeof(_bunnyMaterial.diffuse));
        memcpy(buffer.data() + offset[2], &_bunnyMaterial.specular, sizeof(_bunnyMaterial.specular));
        memcpy(buffer.data() + offset[3], &_bunnyMaterial.shininess, sizeof(_bunnyMaterial.shininess));

        //�������� ����� �� ����������
        glBindBuffer(GL_UNIFORM_BUFFER, _bunnyMaterialUbo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, materialBlockSize, buffer.data());

        //��������� ����� �������, ���� ������
        memcpy(buffer.data() + offset[0], &_cubeMaterial.ambient, sizeof(_cubeMaterial.ambient));
        memcpy(buffer.data() + offset[1], &_cubeMaterial.diffuse, sizeof(_cubeMaterial.diffuse));
        memcpy(buffer.data() + offset[2], &_cubeMaterial.specular, sizeof(_cubeMaterial.specular));
        memcpy(buffer.data() + offset[3], &_cubeMaterial.shininess, sizeof(_cubeMaterial.shininess));

        //�������� ����� �� ����������
        glBindBuffer(GL_UNIFORM_BUFFER, _cubeMaterialUbo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, materialBlockSize, buffer.data());

        //��������� ����� �������, ���� ������
        memcpy(buffer.data() + offset[0], &_sphereMaterial.ambient, sizeof(_sphereMaterial.ambient));
        memcpy(buffer.data() + offset[1], &_sphereMaterial.diffuse, sizeof(_sphereMaterial.diffuse));
        memcpy(buffer.data() + offset[2], &_sphereMaterial.specular, sizeof(_sphereMaterial.specular));
        memcpy(buffer.data() + offset[3], &_sphereMaterial.shininess, sizeof(_sphereMaterial.shininess));

        //�������� ����� �� ����������
        glBindBuffer(GL_UNIFORM_BUFFER, _sphereMaterialUbo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, materialBlockSize, buffer.data());
    }

    void updateGUI() override
    {
        Application::updateGUI();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("MIPT OpenGL Sample", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);

            if (ImGui::CollapsingHeader("Light"))
            {
                ImGui::ColorEdit3("ambient", glm::value_ptr(_light.ambient));
                ImGui::ColorEdit3("diffuse", glm::value_ptr(_light.diffuse));
                ImGui::ColorEdit3("specular", glm::value_ptr(_light.specular));

                ImGui::SliderFloat("radius", &_lr, 0.1f, 10.0f);
                ImGui::SliderFloat("phi", &_phi, 0.0f, 2.0f * glm::pi<float>());
                ImGui::SliderFloat("theta", &_theta, 0.0f, glm::pi<float>());

                ImGui::SliderFloat("attenuation 0", &_light.a0, 0.0f, 10.0f);
                ImGui::SliderFloat("attenuation 1", &_light.a1, 0.0f, 10.0f);
                ImGui::SliderFloat("attenuation 2", &_light.a1, 0.0f, 10.0f);
            }
        }
        ImGui::End();
    }

    void update() override
    {
        Application::update();

        //��������� ���������� Uniform Buffer Object
        glBindBuffer(GL_UNIFORM_BUFFER, _matricesUbo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(_camera), &_camera);

        //----------------------------------

        //������ ����� ���������� ����������� ���������� ��������� � �����
        //������� �������� ������ ������

        GLuint lightBlockIndex = glGetUniformBlockIndex(_shader->id(), "LightInfo");

        GLint lightBlockSize;
        glGetActiveUniformBlockiv(_shader->id(), lightBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &lightBlockSize);

        //������ ����� �������� ������ ���������� ������ ������

        //����� ���������� ���������
        const char* names[7] =
        {
            "LightInfo.pos",
            "LightInfo.La",
            "LightInfo.Ld",
            "LightInfo.Ls",
            "LightInfo.a0",
            "LightInfo.a1",
            "LightInfo.a2",
        };

        GLuint index[7];
        GLint offset[7];

        //����������� ������� 7� �������-����������
        glGetUniformIndices(_shader->id(), 7, names, index);

        //���� �������, ����������� ������ ��� 2� �������-����������
        glGetActiveUniformsiv(_shader->id(), 7, index, GL_UNIFORM_OFFSET, offset);

        //������� ����� � ����������� ������
        std::vector<GLubyte> buffer;
        buffer.resize(lightBlockSize);

        //��������� ����� �������, ��������� ���������� ������

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        glm::vec3 lightPosCamSpace = glm::vec3(_camera.viewMatrix * glm::vec4(_light.position, 1.0));

        memcpy(buffer.data() + offset[0], &lightPosCamSpace, sizeof(lightPosCamSpace));
        memcpy(buffer.data() + offset[1], &_light.ambient, sizeof(_light.ambient));
        memcpy(buffer.data() + offset[2], &_light.diffuse, sizeof(_light.diffuse));
        memcpy(buffer.data() + offset[3], &_light.specular, sizeof(_light.specular));
        memcpy(buffer.data() + offset[4], &_light.a0, sizeof(_light.a0));
        memcpy(buffer.data() + offset[5], &_light.a1, sizeof(_light.a1));
        memcpy(buffer.data() + offset[6], &_light.a2, sizeof(_light.a2));

        //�������� ����� �� ����������
        glBindBuffer(GL_UNIFORM_BUFFER, _lightUbo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, lightBlockSize, buffer.data());
    }

    void draw() override
    {
        //�������� ������� ������� ������ � ��������� �������
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //������� ������ ����� � ������� �� ����������� ���������� ����������� �����
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 lightPos = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * _lr;

        //���������� ������		
        _shader->use();

        // ��������� �� ���������� �������� ������� - ����������
        unsigned int matricesBlockIndex = glGetUniformBlockIndex(_shader->id(), "Matrices");
        glUniformBlockBinding(_shader->id(), matricesBlockIndex, 0); //0� ����� ��������

        unsigned int lightBlockIndex = glGetUniformBlockIndex(_shader->id(), "LightInfo");
        glUniformBlockBinding(_shader->id(), lightBlockIndex, 1); //1� ����� ��������

        unsigned int materialBlockIndex = glGetUniformBlockIndex(_shader->id(), "MaterialInfo");
        glUniformBlockBinding(_shader->id(), materialBlockIndex, 2); //2� ����� ��������

        //��������� �� ���������� ������� ������ ����� � ��������� ���������
        {
            _shader->setMat4Uniform("modelMatrix", _cube->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _cube->modelMatrix()))));

            glBindBufferBase(GL_UNIFORM_BUFFER, 2, _cubeMaterialUbo); //2� ����� ��������

            _cube->draw();
        }

        {
            _shader->setMat4Uniform("modelMatrix", _sphere->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _sphere->modelMatrix()))));

            glBindBufferBase(GL_UNIFORM_BUFFER, 2, _sphereMaterialUbo); //2� ����� ��������

            _sphere->draw();
        }

        {
            _shader->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _bunny->modelMatrix()))));

            glBindBufferBase(GL_UNIFORM_BUFFER, 2, _bunnyMaterialUbo); //2� ����� ��������

            _bunny->draw();
        }

        //������ ������ ��� ��������� �����		
        {
            _markerShader->use();
            _markerShader->setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), lightPos));
            _markerShader->setVec4Uniform("color", glm::vec4(_light.diffuse, 1.0f));
            _marker->draw();
        }
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}