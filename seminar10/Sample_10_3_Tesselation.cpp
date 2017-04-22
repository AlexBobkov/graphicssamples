#include <Application.hpp>
#include <LightInfo.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

#include <glm/gtx/transform.hpp>

#include <iostream>
#include <sstream>
#include <vector>
#include <deque>

/**
Тесселяция
*/
class SampleApplication : public Application
{
public:
    MeshPtr _sphere;

    ShaderProgramPtr _shader;

    //Переменные для управления положением одного источника света
    float _lr = 10.0;
    float _phi = 0.0;
    float _theta = 0.48;

    LightInfo _light;

    //------------------------------

    GLuint _vbo;
    GLuint _vao;

    int outerLevel = 8;
    int innerLevel = 8;

    int maxTessLevel = 0;

    bool dynamicLevel = false;

    void makeScene() override
    {
        Application::makeScene();

        //Максимальное количество текселей в текстурном буфере
        glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);

        std::cout << "Max tesselation level " << maxTessLevel << std::endl;

        //------------------------------

        //std::vector<glm::vec3> vertices =
        //{
        //    glm::vec3(-1.0f, -1.0f, 0.0f),
        //    glm::vec3(1.0f, -1.0f, 0.0f),
        //    glm::vec3(1.0f, 1.0f, 0.0f),
        //    glm::vec3(-1.0f, 1.0f, 0.0f)
        //};

        ////Создаем прямоугольник
        //glGenBuffers(1, &_vbo);
        //glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, vertices.data(), GL_STATIC_DRAW);

        //glGenVertexArrays(1, &_vao);
        //glBindVertexArray(_vao);

        //glEnableVertexAttribArray(0);
        //glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        //glBindVertexArray(0);

        _sphere = makeSphere(0.5f, 5);
        _sphere->setPrimitiveType(GL_PATCHES);

        //=========================================================
        //Инициализация шейдеров

        _shader = std::make_shared<ShaderProgram>();

        ShaderPtr vs = std::make_shared<Shader>(GL_VERTEX_SHADER);
        vs->createFromFile("shaders10/tess.vert");
        _shader->attachShader(vs);

        ShaderPtr tcs = std::make_shared<Shader>(GL_TESS_CONTROL_SHADER);
        tcs->createFromFile("shaders10/tess.control");
        _shader->attachShader(tcs);

        ShaderPtr tes = std::make_shared<Shader>(GL_TESS_EVALUATION_SHADER);
        tes->createFromFile("shaders10/tess.eval");
        _shader->attachShader(tes);

        ShaderPtr fs = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
        fs->createFromFile("shaders10/tess.frag");
        _shader->attachShader(fs);

        _shader->linkProgram();

        //=========================================================
        //Инициализация значений переменных освщения
        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * _lr;
        _light.ambient = glm::vec3(0.2, 0.2, 0.2);
        _light.diffuse = glm::vec3(0.8, 0.8, 0.8);
        _light.specular = glm::vec3(1.0, 1.0, 1.0);
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
            }

            ImGui::SliderInt("outer tess", &outerLevel, 1, maxTessLevel);
            ImGui::SliderInt("inner tess", &innerLevel, 1, maxTessLevel);

            ImGui::Checkbox("dynamic", &dynamicLevel);
        }
        ImGui::End();
    }

    void update()
    {
        Application::update();

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
    }

    void draw() override
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawScene(_shader);

        //Отсоединяем сэмплер и шейдерную программу
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawScene(const ShaderProgramPtr& shader)
    {
        shader->use();

        shader->setMat4Uniform("modelMatrix", glm::mat4(1.0f));
        shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        glm::vec3 lightPosCamSpace = glm::vec3(_camera.viewMatrix * glm::vec4(_light.position, 1.0));
        shader->setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
        shader->setVec3Uniform("light.La", _light.ambient);
        shader->setVec3Uniform("light.Ld", _light.diffuse);
        shader->setVec3Uniform("light.Ls", _light.specular);

        shader->setIntUniform("outerLevel", outerLevel);
        shader->setIntUniform("innerLevel", innerLevel);

        shader->setIntUniform("dynamicLevel", dynamicLevel ? 1 : 0);
                
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        glPatchParameteri(GL_PATCH_VERTICES, 3);

        //glBindVertexArray(_vao);
        //glDrawArrays(GL_PATCHES, 0, 4);
        _sphere->draw();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}