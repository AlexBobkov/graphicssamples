#include <Application.hpp>
#include <LightInfo.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

#include <iostream>
#include <sstream>
#include <vector>

/**
С клавиатуры происходит переключение вариантов минификации. Кнопки 1-6.
*/
class SampleApplication : public Application
{
public:
    MeshPtr _ground;

    //Идентификатор шейдерной программы
    ShaderProgramPtr _shader;

    //Переменные для управления положением одного источника света
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;

    TexturePtr _chessTex;

    int _minificationType;
    //0 - nearest
    //1 - linear
    //2 - nearest mipmap nearest
    //3 - linear mipmap nearest
    //4 - linear mipmap linear
    //5 - anisotropy

    GLuint _samplerMinNearest;
    GLuint _samplerMinLinear;
    GLuint _samplerMinMipNearestNearest;
    GLuint _samplerMinMipLinearNearest;
    GLuint _samplerMinMipLinearLinear;
    GLuint _samplerMinMipLinearLinearAnisotropy;

    void makeScene() override
    {
        Application::makeScene();

        //=========================================================
        //Создание и загрузка мешей		

        _ground = makeGroundPlane(50.0f, 50.0f);

        //=========================================================
        //Инициализация шейдеров

        _shader = std::make_shared<ShaderProgram>();
        _shader->createProgram("shaders5/texture.vert", "shaders5/texture.frag");

        //=========================================================
        //Инициализация значений переменных освщения
        _lr = 3.0;
        _phi = 0.0;
        _theta = glm::pi<float>() * 0.25f;

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _light.ambient = glm::vec3(0.8f);
        _light.diffuse = glm::vec3(0.2f);
        _light.specular = glm::vec3(0.0f);

        //=========================================================
        //Загрузка и создание текстур
        _chessTex = loadTextureDDS("images/chess.dds");

        //=========================================================
        //Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
        glGenSamplers(1, &_samplerMinNearest);
        glSamplerParameteri(_samplerMinNearest, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_samplerMinNearest, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(_samplerMinNearest, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_samplerMinNearest, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenSamplers(1, &_samplerMinLinear);
        glSamplerParameteri(_samplerMinLinear, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_samplerMinLinear, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_samplerMinLinear, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_samplerMinLinear, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenSamplers(1, &_samplerMinMipNearestNearest);
        glSamplerParameteri(_samplerMinMipNearestNearest, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_samplerMinMipNearestNearest, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glSamplerParameteri(_samplerMinMipNearestNearest, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_samplerMinMipNearestNearest, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenSamplers(1, &_samplerMinMipLinearNearest);
        glSamplerParameteri(_samplerMinMipLinearNearest, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_samplerMinMipLinearNearest, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glSamplerParameteri(_samplerMinMipLinearNearest, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_samplerMinMipLinearNearest, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenSamplers(1, &_samplerMinMipLinearLinear);
        glSamplerParameteri(_samplerMinMipLinearLinear, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_samplerMinMipLinearLinear, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(_samplerMinMipLinearLinear, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_samplerMinMipLinearLinear, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenSamplers(1, &_samplerMinMipLinearLinearAnisotropy);
        glSamplerParameteri(_samplerMinMipLinearLinearAnisotropy, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_samplerMinMipLinearLinearAnisotropy, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(_samplerMinMipLinearLinearAnisotropy, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_samplerMinMipLinearLinearAnisotropy, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glSamplerParameterf(_samplerMinMipLinearLinearAnisotropy, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);

        _minificationType = 0;

        //====
        GLfloat maxAniso = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);

        std::cout << "Max anistropy " << maxAniso << std::endl;
        //====
    }

    void updateGUI() override
    {
        Application::updateGUI();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("MIPT OpenGL Sample", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);

            ImGui::RadioButton("nearest", &_minificationType, 0);
            ImGui::RadioButton("linear", &_minificationType, 1);
            ImGui::RadioButton("nearest mip nearest", &_minificationType, 2);
            ImGui::RadioButton("linear mip nearest", &_minificationType, 3);
            ImGui::RadioButton("linear mip linear", &_minificationType, 4);
            ImGui::RadioButton("anisotropy", &_minificationType, 5);
        }
        ImGui::End();
    }

    void handleKey(int key, int scancode, int action, int mods) override
    {
        Application::handleKey(key, scancode, action, mods);

        if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_1)
            {
                _minificationType = 0;
            }
            else if (key == GLFW_KEY_2)
            {
                _minificationType = 1;
            }
            else if (key == GLFW_KEY_3)
            {
                _minificationType = 2;
            }
            else if (key == GLFW_KEY_4)
            {
                _minificationType = 3;
            }
            else if (key == GLFW_KEY_5)
            {
                _minificationType = 4;
            }
            else if (key == GLFW_KEY_6)
            {
                _minificationType = 5;
            }
        }
    }

    void draw() override
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Подключаем шейдер		
        _shader->use();

        //Загружаем на видеокарту значения юниформ-переменных
        _shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        glm::vec3 lightPosCamSpace = glm::vec3(_camera.viewMatrix * glm::vec4(_light.position, 1.0));

        _shader->setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
        _shader->setVec3Uniform("light.La", _light.ambient);
        _shader->setVec3Uniform("light.Ld", _light.diffuse);
        _shader->setVec3Uniform("light.Ls", _light.specular);

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0        
        if (_minificationType == 0)
        {
            glBindSampler(0, _samplerMinNearest);
        }
        else if (_minificationType == 1)
        {
            glBindSampler(0, _samplerMinLinear);
        }
        else if (_minificationType == 2)
        {
            glBindSampler(0, _samplerMinMipNearestNearest);
        }
        else if (_minificationType == 3)
        {
            glBindSampler(0, _samplerMinMipLinearNearest);
        }
        else if (_minificationType == 4)
        {
            glBindSampler(0, _samplerMinMipLinearLinear);
        }
        else
        {
            glBindSampler(0, _samplerMinMipLinearLinearAnisotropy);
        }
        _chessTex->bind();
        _shader->setIntUniform("diffuseTex", 0);

        //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
        {
            _shader->setMat4Uniform("modelMatrix", _ground->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _ground->modelMatrix()))));

            _ground->draw();
        }

        //Отсоединяем сэмплер и шейдерную программу
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