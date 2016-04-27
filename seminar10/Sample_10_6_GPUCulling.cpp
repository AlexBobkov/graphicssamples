#include <Application.hpp>
#include <LightInfo.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/gtx/transform.hpp>

#include <iostream>
#include <sstream>
#include <vector>
#include <deque>

namespace
{
    float frand()
    {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
}

/**
Инстансинг: отсечение на видеокарте (gpu culling)
*/
class SampleApplication : public Application
{
public:
    MeshPtr _teapot;

    TexturePtr _brickTex;
    GLuint _sampler;

    //------------------------

    ShaderProgramPtr _shader;
    ShaderProgramPtr _cullShader;

    //------------------------

    //Переменные для управления положением одного источника света
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;

    //------------------------

    const unsigned int K = 1000; //Количество инстансов

    std::vector<glm::vec3> _positionsVec3;

    DataBufferPtr _bufVec3;

    TexturePtr _bufferTex;

    //------------------------

    GLuint _TF; //Объект для хранения настроект Transform Feedback
    GLuint _cullVao; //VAO, который хранит настройки буфера для чтения данных во время Transform Feedback
    GLuint _tfOutputVbo; //VBO, в который будут записываться смещения моделей после отсечения

    GLuint _query; //Переменная-счетчик, куда будет записываться количество пройденных отбор моделей

    void makeScene() override
    {
        Application::makeScene();

        //Максимальное количество вершиных атрибутов, выраженное в количестве vec4
        GLint maxVertexAttributes;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttributes);

        //Максимальный размер всех юниформ-перемнных, выраженный в количестве vec4
        GLint maxVertexUniformVectors;
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertexUniformVectors);

        //Максимальный размер юниформ-блока в байтах
        GLint maxUniformBlockSize;
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBlockSize);

        //Максимальный размер ShaderStorage-блока в байтах
        GLint maxShaderStorageBlockSize;
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxShaderStorageBlockSize);

        //Максимальное количество текселей в текстурном буфере
        GLint maxTextureBufferSize;
        glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &maxTextureBufferSize);

        std::cout << "GL_MAX_VERTEX_ATTRIBS " << maxVertexAttributes << std::endl;
        std::cout << "GL_MAX_VERTEX_UNIFORM_VECTORS " << maxVertexUniformVectors << std::endl;
        std::cout << "GL_MAX_UNIFORM_BLOCK_SIZE " << maxUniformBlockSize << std::endl;
        std::cout << "GL_MAX_SHADER_STORAGE_BLOCK_SIZE " << maxShaderStorageBlockSize << std::endl;
        std::cout << "GL_MAX_TEXTURE_BUFFER_SIZE " << maxTextureBufferSize << std::endl;

        //=========================================================
        //Инициализируем K случайных сдвигов для K экземпляров
        //Некоторые варианты требуют выровненный массив по vec4, а некоторые нет

        srand((int)(glfwGetTime() * 1000));

        float size = 50.0f;
        for (unsigned int i = 0; i < K; i++)
        {
            _positionsVec3.push_back(glm::vec3(frand() * size - 0.5 * size, frand() * size - 0.5 * size, 0.0));
        }

        //Создаем буферы без выравнивания (_bufVec3) и с выравниванием (_bufVec4)

        _bufVec3 = std::make_shared<DataBuffer>(GL_ARRAY_BUFFER);
        _bufVec3->setData(_positionsVec3.size() * sizeof(float) * 3, _positionsVec3.data());

        //----------------------------

        //Инициализируем буфер, в который будут скопированы смещения моделей после отсечения
        glGenBuffers(1, &_tfOutputVbo);
        glBindBuffer(GL_ARRAY_BUFFER, _tfOutputVbo);
        glBufferData(GL_ARRAY_BUFFER, _positionsVec3.size() * sizeof(float) * 3, 0, GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //----------------------------

        //Создаем текстурный буфер и привязываем к нему буфер без выравнивания
        _bufferTex = std::make_shared<Texture>(GL_TEXTURE_BUFFER);
        _bufferTex->bind();
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F_ARB, _tfOutputVbo);
        _bufferTex->unbind();

        //----------------------------

        _teapot = loadFromFile("models/teapot.obj");

        //=========================================================
        //Инициализация шейдеров

        _shader = std::make_shared<ShaderProgram>();
        _shader->createProgram("shaders10/instancingTexture.vert", "shaders/common.frag");

        //----------------------------

        _cullShader = std::make_shared<ShaderProgram>();

        ShaderPtr vs = std::make_shared<Shader>(GL_VERTEX_SHADER);
        vs->createFromFile("shaders10/cull.vert");
        _cullShader->attachShader(vs);

        ShaderPtr gs = std::make_shared<Shader>(GL_GEOMETRY_SHADER);
        gs->createFromFile("shaders10/cull.geom");
        _cullShader->attachShader(gs);

        ShaderPtr fs = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
        fs->createFromFile("shaders10/cull.frag");
        _cullShader->attachShader(fs);

        //Выходные переменные, которые будут записаны в буфер
        const char* attribs[] = { "position" };
        glTransformFeedbackVaryings(_cullShader->id(), 1, attribs, GL_SEPARATE_ATTRIBS);

        _cullShader->linkProgram();
                
        //----------------------------

        //VAO, который будет поставлять данные для отсечения
        glGenVertexArrays(1, &_cullVao);
        glBindVertexArray(_cullVao);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, _bufVec3->id());
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindVertexArray(0);

        //----------------------------
        
        //Настроечный объект Transform Feedback
        glGenTransformFeedbacks(1, &_TF);        
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _TF);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _tfOutputVbo);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);        

        //----------------------------

        glGenQueries(1, &_query);
        
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
        _brickTex = loadTexture("images/brick.jpg");

        //=========================================================
        //Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
        glGenSamplers(1, &_sampler);
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
                
        cull(_cullShader);

        drawScene(_shader);

        //Отсоединяем сэмплер и шейдерную программу
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void cull(const ShaderProgramPtr& shader)
    {
        shader->use();        

        shader->setMat4Uniform("modelMatrix", glm::mat4(1.0f));
        shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);
                
        glEnable(GL_RASTERIZER_DISCARD);
                
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _TF);                        

        glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, _query);

        glBeginTransformFeedback(GL_POINTS);
        
        glBindVertexArray(_cullVao);
        glDrawArrays(GL_POINTS, 0, _positionsVec3.size());
                
        glEndTransformFeedback();

        glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

        glDisable(GL_RASTERIZER_DISCARD);
    }

    void drawScene(const ShaderProgramPtr& shader)
    {
        GLuint primitivesWritten;
        glGetQueryObjectuiv(_query, GL_QUERY_RESULT, &primitivesWritten);

        shader->use();

        //Загружаем на видеокарту значения юниформ-переменных
        shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        glm::vec3 lightPosCamSpace = glm::vec3(_camera.viewMatrix * glm::vec4(_light.position, 1.0));
        shader->setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
        shader->setVec3Uniform("light.La", _light.ambient);
        shader->setVec3Uniform("light.Ld", _light.diffuse);
        shader->setVec3Uniform("light.Ls", _light.specular);

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0        
        glBindSampler(0, _sampler);
        _brickTex->bind();
        shader->setIntUniform("diffuseTex", 0);

        glActiveTexture(GL_TEXTURE1);
        _bufferTex->bind();
        shader->setIntUniform("texBuf", 1);

        shader->setMat4Uniform("modelMatrix", glm::mat4(1.0));
        shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix))));

        _teapot->drawInstanced(primitivesWritten);
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}