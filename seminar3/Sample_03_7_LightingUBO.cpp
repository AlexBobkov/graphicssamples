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
    float a0 = 1.0f;
    float a1 = 0.0f;
    float a2 = 0.0f;
};

struct MaterialInfo
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess = 128.0f;
};

/**
Вариант с хранением параметров освещения в Uniform Buffer Object
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;

    MeshPtr _marker; //Маркер для источника света

    ShaderProgramPtr _shader;
    ShaderProgramPtr _markerShader;

    //Координаты источника света
    float _lr = 5.0;
    float _phi = 0.0;
    float _theta = glm::pi<float>() * 0.25f;

    //Параметры источника света
    LightInfo _light;

    //Параметры материалов
    MaterialInfo _bunnyMaterial;
    MaterialInfo _cubeMaterial;
    MaterialInfo _sphereMaterial;

    //Буферы юниформ-переменных (Uniform Buffer Object)
    GLuint _matricesUbo;
    GLuint _lightUbo;
    GLuint _bunnyMaterialUbo;
    GLuint _cubeMaterialUbo;
    GLuint _sphereMaterialUbo;

    void makeScene() override
    {
        Application::makeScene();

        //=========================================================
        //Создание и загрузка мешей		

        _cube = makeCube(0.5f);
        _cube->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f)));

        _sphere = makeSphere(0.5f);
        _sphere->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f)));

        _bunny = loadFromFile("models/bunny.obj");
        _bunny->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        _marker = makeSphere(0.1f);

        //=========================================================
        //Инициализация шейдеров

        _shader = std::make_shared<ShaderProgram>("shaders3/lightingUBO.vert", "shaders3/lightingUBO.frag");
        _markerShader = std::make_shared<ShaderProgram>("shaders/marker.vert", "shaders/marker.frag");

        //=========================================================
        //Инициализация значений переменных освщения
        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _light.ambient = glm::vec3(0.2, 0.2, 0.2);
        _light.diffuse = glm::vec3(0.8, 0.8, 0.8);
        _light.specular = glm::vec3(1.0, 1.0, 1.0);
        _light.a0 = 1.0f;
        _light.a1 = 0.0f;
        _light.a2 = 0.0f;

        //Инициализация материалов
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
        //Инициализация Uniform Buffer Object

        //Сначала создаем UBO для матриц проекции и вида
        //Поскольку матрицы расположены в памяти просто без промежутков, то можно вычислить размер массива с помощью sizeof(_camera)

        glGenBuffers(1, &_matricesUbo);
        glBindBuffer(GL_UNIFORM_BUFFER, _matricesUbo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(_camera), nullptr, GL_DYNAMIC_DRAW); //Выделяем память, пока ничего не копируем
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        //Привязываем к 0й точке привязки
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _matricesUbo);

        //--------------------------

        //Создаем UBO для источника света
        //Переменные источника света располагаются в памяти с промежутками,
        //поэтому сначала нужно запросить размер буфера с помощью функции glGetActiveUniformBlockiv

        GLuint lightBlockIndex = glGetUniformBlockIndex(_shader->id(), "LightInfo");

        GLint lightBlockSize;
        glGetActiveUniformBlockiv(_shader->id(), lightBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &lightBlockSize);

        glGenBuffers(1, &_lightUbo);
        glBindBuffer(GL_UNIFORM_BUFFER, _lightUbo);
        glBufferData(GL_UNIFORM_BUFFER, lightBlockSize, nullptr, GL_DYNAMIC_DRAW); //Выделяем память, пока ничего не копируем
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        //Привязываем к 1й точке привязки
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, _lightUbo);

        //--------------------------

        //Создаем UBO для материала
        //Переменные материала располагаются в памяти с промежутками,
        //поэтому сначала нужно запросить размер буфера с помощью функции glGetActiveUniformBlockiv

        GLuint materialBlockIndex = glGetUniformBlockIndex(_shader->id(), "MaterialInfo");

        GLint materialBlockSize;
        glGetActiveUniformBlockiv(_shader->id(), materialBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &materialBlockSize);

        glGenBuffers(1, &_bunnyMaterialUbo);
        glBindBuffer(GL_UNIFORM_BUFFER, _bunnyMaterialUbo);
        glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glGenBuffers(1, &_cubeMaterialUbo);
        glBindBuffer(GL_UNIFORM_BUFFER, _cubeMaterialUbo);
        glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glGenBuffers(1, &_sphereMaterialUbo);
        glBindBuffer(GL_UNIFORM_BUFFER, _sphereMaterialUbo);
        glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        //Для материала пока не назначаем точку привязки, т.к. материалов будет несколько, и будем их менять

        //Пусть свойства материала будут статичными. Тогда можно заполнить буфер данными прямо здесь
        //Для этого нужно получить сдвиги переменных внутри буфера

        //Имена переменных материала
        const char* names[4] =
        {
            "MaterialInfo.Ka",
            "MaterialInfo.Kd",
            "MaterialInfo.Ks",
            "MaterialInfo.shininess"
        };

        GLuint index[4];
        GLint offset[4];

        //Запрашиваем индексы 4х юниформ-переменных
        glGetUniformIndices(_shader->id(), 4, names, index);

        //Зная индексы, запрашиваем сдвиги для 4х юниформ-переменных
        glGetActiveUniformsiv(_shader->id(), 4, index, GL_UNIFORM_OFFSET, offset);

        //Создаем буфер в оперативной памяти
        std::vector<GLubyte> buffer;
        buffer.resize(materialBlockSize);

        //Заполняем буфер данными, зная сдвиги
        memcpy(buffer.data() + offset[0], &_bunnyMaterial.ambient, sizeof(_bunnyMaterial.ambient));
        memcpy(buffer.data() + offset[1], &_bunnyMaterial.diffuse, sizeof(_bunnyMaterial.diffuse));
        memcpy(buffer.data() + offset[2], &_bunnyMaterial.specular, sizeof(_bunnyMaterial.specular));
        memcpy(buffer.data() + offset[3], &_bunnyMaterial.shininess, sizeof(_bunnyMaterial.shininess));

        //Копируем буфер на видеокарту
        glBindBuffer(GL_UNIFORM_BUFFER, _bunnyMaterialUbo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, materialBlockSize, buffer.data());

        //Заполняем буфер данными, зная сдвиги
        memcpy(buffer.data() + offset[0], &_cubeMaterial.ambient, sizeof(_cubeMaterial.ambient));
        memcpy(buffer.data() + offset[1], &_cubeMaterial.diffuse, sizeof(_cubeMaterial.diffuse));
        memcpy(buffer.data() + offset[2], &_cubeMaterial.specular, sizeof(_cubeMaterial.specular));
        memcpy(buffer.data() + offset[3], &_cubeMaterial.shininess, sizeof(_cubeMaterial.shininess));

        //Копируем буфер на видеокарту
        glBindBuffer(GL_UNIFORM_BUFFER, _cubeMaterialUbo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, materialBlockSize, buffer.data());

        //Заполняем буфер данными, зная сдвиги
        memcpy(buffer.data() + offset[0], &_sphereMaterial.ambient, sizeof(_sphereMaterial.ambient));
        memcpy(buffer.data() + offset[1], &_sphereMaterial.diffuse, sizeof(_sphereMaterial.diffuse));
        memcpy(buffer.data() + offset[2], &_sphereMaterial.specular, sizeof(_sphereMaterial.specular));
        memcpy(buffer.data() + offset[3], &_sphereMaterial.shininess, sizeof(_sphereMaterial.shininess));

        //Копируем буфер на видеокарту
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
                ImGui::SliderFloat("attenuation 2", &_light.a2, 0.0f, 10.0f);
            }
        }
        ImGui::End();
    }

    void update() override
    {
        Application::update();

        //Обновляем содержимое Uniform Buffer Object
        glBindBuffer(GL_UNIFORM_BUFFER, _matricesUbo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(_camera), &_camera);

        //----------------------------------

        //Теперь нужно перезалить обновленные переменные освещения в буфер
        //Сначала получаем размер буфера

        GLuint lightBlockIndex = glGetUniformBlockIndex(_shader->id(), "LightInfo");

        GLint lightBlockSize;
        glGetActiveUniformBlockiv(_shader->id(), lightBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &lightBlockSize);

        //Теперь нужно получить сдвиги переменных внутри буфера

        //Имена переменных освещения
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

        //Запрашиваем индексы 7х юниформ-переменных
        glGetUniformIndices(_shader->id(), 7, names, index);

        //Зная индексы, запрашиваем сдвиги для 2х юниформ-переменных
        glGetActiveUniformsiv(_shader->id(), 7, index, GL_UNIFORM_OFFSET, offset);

        //Создаем буфер в оперативной памяти
        std::vector<GLubyte> buffer;
        buffer.resize(lightBlockSize);

        //Заполняем буфер данными, используя полученные сдвиги

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * _lr;
        glm::vec3 lightPosCamSpace = glm::vec3(_camera.viewMatrix * glm::vec4(_light.position, 1.0));

        memcpy(buffer.data() + offset[0], &lightPosCamSpace, sizeof(lightPosCamSpace));
        memcpy(buffer.data() + offset[1], &_light.ambient, sizeof(_light.ambient));
        memcpy(buffer.data() + offset[2], &_light.diffuse, sizeof(_light.diffuse));
        memcpy(buffer.data() + offset[3], &_light.specular, sizeof(_light.specular));
        memcpy(buffer.data() + offset[4], &_light.a0, sizeof(_light.a0));
        memcpy(buffer.data() + offset[5], &_light.a1, sizeof(_light.a1));
        memcpy(buffer.data() + offset[6], &_light.a2, sizeof(_light.a2));

        //Копируем буфер на видеокарту
        glBindBuffer(GL_UNIFORM_BUFFER, _lightUbo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, lightBlockSize, buffer.data());
    }

    void draw() override
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 lightPos = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * _lr;

        //Подключаем шейдер		
        _shader->use();

        // Загружаем на видеокарту значения юниформ - переменных
        unsigned int matricesBlockIndex = glGetUniformBlockIndex(_shader->id(), "Matrices");
        glUniformBlockBinding(_shader->id(), matricesBlockIndex, 0); //0я точка привязки

        unsigned int lightBlockIndex = glGetUniformBlockIndex(_shader->id(), "LightInfo");
        glUniformBlockBinding(_shader->id(), lightBlockIndex, 1); //1я точка привязки

        unsigned int materialBlockIndex = glGetUniformBlockIndex(_shader->id(), "MaterialInfo");
        glUniformBlockBinding(_shader->id(), materialBlockIndex, 2); //2я точка привязки

        //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
        {
            _shader->setMat4Uniform("modelMatrix", _cube->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _cube->modelMatrix()))));

            glBindBufferBase(GL_UNIFORM_BUFFER, 2, _cubeMaterialUbo); //2я точка привязки

            _cube->draw();
        }

        {
            _shader->setMat4Uniform("modelMatrix", _sphere->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _sphere->modelMatrix()))));

            glBindBufferBase(GL_UNIFORM_BUFFER, 2, _sphereMaterialUbo); //2я точка привязки

            _sphere->draw();
        }

        {
            _shader->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _bunny->modelMatrix()))));

            glBindBufferBase(GL_UNIFORM_BUFFER, 2, _bunnyMaterialUbo); //2я точка привязки

            _bunny->draw();
        }

        //Рисуем маркер для источника света		
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