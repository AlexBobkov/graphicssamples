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
    float _lr = 3.0;
    float _phi = 0.0;
    float _theta = glm::pi<float>() * 0.25f;

    LightInfo _light;

    TexturePtr _chessTex;

    enum MinificationType : int
    {
        Nearest,
        Linear,
        NearestMipmapNearest,
        LinearMipmapNearest,
        LinearMipmapLinear,
        Anisotropy
    };

    MinificationType _minificationType = Nearest;
    GLuint _samplers[6];

    void makeScene() override
    {
        Application::makeScene();

        //=========================================================
        //Создание и загрузка мешей		

        _ground = makeGroundPlane(50.0f, 50.0f);

        //=========================================================
        //Инициализация шейдеров

        _shader = std::make_shared<ShaderProgram>("shaders4/texture.vert", "shaders4/texture.frag");

        //=========================================================
        //Инициализация значений переменных освщения
        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * _lr;
        _light.ambient = glm::vec3(0.8f);
        _light.diffuse = glm::vec3(0.2f);
        _light.specular = glm::vec3(0.0f);

        //=========================================================
        //Загрузка и создание текстур
        _chessTex = loadTextureDDS("images/chess.dds");

        //=========================================================
        //Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
        glGenSamplers(6, _samplers);

        glSamplerParameteri(_samplers[0], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_samplers[0], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(_samplers[0], GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_samplers[0], GL_TEXTURE_WRAP_T, GL_REPEAT);

        glSamplerParameteri(_samplers[1], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_samplers[1], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_samplers[1], GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_samplers[1], GL_TEXTURE_WRAP_T, GL_REPEAT);

        glSamplerParameteri(_samplers[2], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_samplers[2], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glSamplerParameteri(_samplers[2], GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_samplers[2], GL_TEXTURE_WRAP_T, GL_REPEAT);

        glSamplerParameteri(_samplers[3], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_samplers[3], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glSamplerParameteri(_samplers[3], GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_samplers[3], GL_TEXTURE_WRAP_T, GL_REPEAT);

        glSamplerParameteri(_samplers[4], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_samplers[4], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(_samplers[4], GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_samplers[4], GL_TEXTURE_WRAP_T, GL_REPEAT);

        glSamplerParameteri(_samplers[5], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_samplers[5], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(_samplers[5], GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_samplers[5], GL_TEXTURE_WRAP_T, GL_REPEAT);
        glSamplerParameterf(_samplers[5], GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);

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

            ImGui::RadioButton("nearest", reinterpret_cast<int*>(&_minificationType), Nearest);
            ImGui::RadioButton("linear", reinterpret_cast<int*>(&_minificationType), Linear);
            ImGui::RadioButton("nearest mip nearest", reinterpret_cast<int*>(&_minificationType), NearestMipmapNearest);
            ImGui::RadioButton("linear mip nearest", reinterpret_cast<int*>(&_minificationType), LinearMipmapNearest);
            ImGui::RadioButton("linear mip linear", reinterpret_cast<int*>(&_minificationType), LinearMipmapLinear);
            ImGui::RadioButton("anisotropy", reinterpret_cast<int*>(&_minificationType), Anisotropy);
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
                _minificationType = Nearest;
            }
            else if (key == GLFW_KEY_2)
            {
                _minificationType = Linear;
            }
            else if (key == GLFW_KEY_3)
            {
                _minificationType = NearestMipmapNearest;
            }
            else if (key == GLFW_KEY_4)
            {
                _minificationType = LinearMipmapNearest;
            }
            else if (key == GLFW_KEY_5)
            {
                _minificationType = LinearMipmapLinear;
            }
            else if (key == GLFW_KEY_6)
            {
                _minificationType = Anisotropy;
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

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * _lr;
        glm::vec3 lightPosCamSpace = glm::vec3(_camera.viewMatrix * glm::vec4(_light.position, 1.0));

        _shader->setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
        _shader->setVec3Uniform("light.La", _light.ambient);
        _shader->setVec3Uniform("light.Ld", _light.diffuse);
        _shader->setVec3Uniform("light.Ls", _light.specular);

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0        
        glBindSampler(0, _samplers[_minificationType]);
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