#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
�������� �������� �����. 2 �������� ��������: � ���������� � ������������ ����������
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;

    MeshPtr _marker; //������ ��� ��������� �����

    ShaderProgramPtr _shaderPerVertex;
    ShaderProgramPtr _shaderPerFragment;
    ShaderProgramPtr _markerShader;

    //���������� ��������� �����
    float _lr = 5.0;
    float _phi = 0.0;
    float _theta = glm::pi<float>() * 0.25f;

    //��������� ��������� �����
    glm::vec3 _lightAmbientColor;
    glm::vec3 _lightDiffuseColor;
    glm::vec3 _lightSpecularColor;

    //��������� ���������
    glm::vec3 _bunnyAmbientColor;
    glm::vec3 _bunnyDiffuseColor;
    glm::vec3 _bunnySpecularColor;
    float _bunnyShininess;

    void makeScene() override
    {
        Application::makeScene();

        //=========================================================
        //�������� � �������� �����		

        _cube = makeCube(0.5f);
        _cube->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f)));

        _sphere = makeSphere(0.5f, 40);
        _sphere->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f)));

        _bunny = loadFromFile("models/bunny.obj");
        _bunny->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        _marker = makeSphere(0.1f);

        //=========================================================
        //������������� ��������

        _shaderPerVertex = std::make_shared<ShaderProgram>("shaders3/specularPointLightPerVertex.vert", "shaders3/specularPointLightPerVertex.frag");
        _shaderPerFragment = std::make_shared<ShaderProgram>("shaders3/specularPointLightPerFragment.vert", "shaders3/specularPointLightPerFragment.frag");
        _markerShader = std::make_shared<ShaderProgram>("shaders/marker.vert", "shaders/marker.frag");

        //=========================================================
        //������������� �������� ���������� ��������
        _lightAmbientColor = glm::vec3(0.2, 0.2, 0.2);
        _lightDiffuseColor = glm::vec3(0.8, 0.8, 0.8);
        _lightSpecularColor = glm::vec3(1.0, 1.0, 1.0);

        //������������� ��������� �������
        _bunnyAmbientColor = glm::vec3(1.0, 1.0, 0.0);
        _bunnyDiffuseColor = glm::vec3(1.0, 1.0, 0.0);
        _bunnySpecularColor = glm::vec3(1.0, 1.0, 1.0);
        _bunnyShininess = 128.0f;
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
                ImGui::ColorEdit3("ambient", glm::value_ptr(_lightAmbientColor));
                ImGui::ColorEdit3("diffuse", glm::value_ptr(_lightDiffuseColor));
                ImGui::ColorEdit3("specular", glm::value_ptr(_lightSpecularColor));

                ImGui::SliderFloat("radius", &_lr, 0.1f, 10.0f);
                ImGui::SliderFloat("phi", &_phi, 0.0f, 2.0f * glm::pi<float>());
                ImGui::SliderFloat("theta", &_theta, 0.0f, glm::pi<float>());
            }

            if (ImGui::CollapsingHeader("Rabbit material"))
            {
                ImGui::ColorEdit3("mat ambient", glm::value_ptr(_bunnyAmbientColor));
                ImGui::ColorEdit3("mat diffuse", glm::value_ptr(_bunnyDiffuseColor));
                ImGui::ColorEdit3("mat specular", glm::value_ptr(_bunnySpecularColor));
                ImGui::SliderFloat("shininess", &_bunnyShininess, 0.1f, 255.0f);
            }
        }
        ImGui::End();
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

        //������� ��������� ������ � ����������� ����������
        {
            //���������� ������		
            _shaderPerVertex->use();

            //��������� �� ���������� �������� �������-����������
            _shaderPerVertex->setMat4Uniform("viewMatrix", _camera.viewMatrix);
            _shaderPerVertex->setMat4Uniform("projectionMatrix", _camera.projMatrix);

            _shaderPerVertex->setVec3Uniform("light.pos", lightPos);
            _shaderPerVertex->setVec3Uniform("light.La", _lightAmbientColor);
            _shaderPerVertex->setVec3Uniform("light.Ld", _lightDiffuseColor);
            _shaderPerVertex->setVec3Uniform("light.Ls", _lightSpecularColor);

            //��������� �� ���������� ������� ������ ����� � ��������� ���������
            {
                _shaderPerVertex->setMat4Uniform("modelMatrix", _cube->modelMatrix());
                _shaderPerVertex->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _cube->modelMatrix()))));

                _shaderPerVertex->setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
                _shaderPerVertex->setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));
                _shaderPerVertex->setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerVertex->setFloatUniform("material.shininess", _bunnyShininess);

                _cube->draw();
            }

            {
                _shaderPerVertex->setMat4Uniform("modelMatrix", _sphere->modelMatrix());
                _shaderPerVertex->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _sphere->modelMatrix()))));

                _shaderPerVertex->setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerVertex->setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerVertex->setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerVertex->setFloatUniform("material.shininess", _bunnyShininess);

                _sphere->draw();
            }

            {
                _shaderPerVertex->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
                _shaderPerVertex->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _bunny->modelMatrix()))));

                _shaderPerVertex->setVec3Uniform("material.Ka", glm::vec3(_bunnyAmbientColor));
                _shaderPerVertex->setVec3Uniform("material.Kd", glm::vec3(_bunnyDiffuseColor));
                _shaderPerVertex->setVec3Uniform("material.Ks", glm::vec3(_bunnySpecularColor));
                _shaderPerVertex->setFloatUniform("material.shininess", _bunnyShininess);

                _bunny->draw();
            }
        }

        //������� ��������� ������ � ������������� ����������
        {
            //���������� ������		
            _shaderPerFragment->use();

            //��������� �� ���������� �������� �������-����������
            _shaderPerFragment->setMat4Uniform("viewMatrix", _camera.viewMatrix);
            _shaderPerFragment->setMat4Uniform("projectionMatrix", _camera.projMatrix);


            _shaderPerFragment->setVec3Uniform("light.pos", lightPos);
            _shaderPerFragment->setVec3Uniform("light.La", _lightAmbientColor);
            _shaderPerFragment->setVec3Uniform("light.Ld", _lightDiffuseColor);
            _shaderPerFragment->setVec3Uniform("light.Ls", _lightSpecularColor);

            //��������� �� ���������� ������� ������ ����� � ��������� ���������
            {
                _shaderPerFragment->setMat4Uniform("modelMatrix", glm::translate(_cube->modelMatrix(), glm::vec3(1.5f, 0.0f, 0.0f)));
                _shaderPerFragment->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _cube->modelMatrix()))));

                _shaderPerFragment->setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
                _shaderPerFragment->setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));
                _shaderPerFragment->setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerFragment->setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerFragment->setFloatUniform("material.shininess", _bunnyShininess);

                _cube->draw();
            }

            {
                _shaderPerFragment->setMat4Uniform("modelMatrix", glm::translate(_sphere->modelMatrix(), glm::vec3(1.5f, 0.0f, 0.0f)));
                _shaderPerFragment->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _sphere->modelMatrix()))));

                _shaderPerFragment->setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerFragment->setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerFragment->setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerFragment->setFloatUniform("material.shininess", _bunnyShininess);

                _sphere->draw();
            }

            {
                _shaderPerFragment->setMat4Uniform("modelMatrix", glm::translate(_bunny->modelMatrix(), glm::vec3(1.5f, 0.0f, 0.0f)));
                _shaderPerFragment->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _bunny->modelMatrix()))));

                _shaderPerFragment->setVec3Uniform("material.Ka", glm::vec3(_bunnyAmbientColor));
                _shaderPerFragment->setVec3Uniform("material.Kd", glm::vec3(_bunnyDiffuseColor));
                _shaderPerFragment->setVec3Uniform("material.Ks", glm::vec3(_bunnySpecularColor));
                _shaderPerFragment->setFloatUniform("material.shininess", _bunnyShininess);

                _bunny->draw();
            }
        }

        //������ ������ ��� ��������� �����		
        {
            _markerShader->use();
            _markerShader->setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), lightPos));
            _markerShader->setVec4Uniform("color", glm::vec4(_lightDiffuseColor, 1.0f));
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