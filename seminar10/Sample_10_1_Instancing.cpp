#include <Application.hpp>
#include <LightInfo.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

    /**
    Загружает 3д-модель, размножает её в буфере positions.size() раз, сдвигая координаты в мировой системе координат
    */
    MeshPtr loadFromFileArray(const std::string& filename, const std::vector<glm::vec3>& positions)
    {
        const struct aiScene* assimpScene = aiImportFile(filename.c_str(), aiProcess_Triangulate);

        if (!assimpScene)
        {
            std::cerr << aiGetErrorString() << std::endl;
            return std::make_shared<Mesh>();
        }

        if (assimpScene->mNumMeshes == 0)
        {
            std::cerr << "There is no meshes in file " << filename << std::endl;
            return std::make_shared<Mesh>();
        }

        const struct aiMesh* assimpMesh = assimpScene->mMeshes[0];

        if (!assimpMesh->HasPositions())
        {
            std::cerr << "This demo does not support meshes without positions\n";
            return std::make_shared<Mesh>();
        }

        if (!assimpMesh->HasNormals())
        {
            std::cerr << "This demo does not support meshes without normals\n";
            return std::make_shared<Mesh>();
        }

        if (!assimpMesh->HasTextureCoords(0))
        {
            std::cerr << "This demo does not support meshes without texcoords for texture unit 0\n";
            return std::make_shared<Mesh>();
        }

        unsigned int instanceCount = positions.size();

        std::vector<glm::vec3> vertices(assimpMesh->mNumVertices * instanceCount);
        std::vector<glm::vec3> normals(assimpMesh->mNumVertices * instanceCount);
        std::vector<glm::vec2> texcoords(assimpMesh->mNumVertices * instanceCount);

        for (unsigned int k = 0; k < positions.size(); k++)
        {
            for (unsigned int i = 0; i < assimpMesh->mNumVertices; i++)
            {
                const aiVector3D* vp = &(assimpMesh->mVertices[i]);
                const aiVector3D* normal = &(assimpMesh->mNormals[i]);
                const aiVector3D* tc = &(assimpMesh->mTextureCoords[0][i]);

                vertices[i + k * assimpMesh->mNumVertices] = glm::vec3(vp->x, vp->y, vp->z) + positions[k];
                normals[i + k * assimpMesh->mNumVertices] = glm::vec3(normal->x, normal->y, normal->z);
                texcoords[i + k * assimpMesh->mNumVertices] = glm::vec2(tc->x, tc->y);
            }
        }

        DataBufferPtr buf0 = std::make_shared<DataBuffer>(GL_ARRAY_BUFFER);
        buf0->setData(vertices.size() * sizeof(float) * 3, vertices.data());

        DataBufferPtr buf1 = std::make_shared<DataBuffer>(GL_ARRAY_BUFFER);
        buf1->setData(normals.size() * sizeof(float) * 3, normals.data());

        DataBufferPtr buf2 = std::make_shared<DataBuffer>(GL_ARRAY_BUFFER);
        buf2->setData(texcoords.size() * sizeof(float) * 2, texcoords.data());

        MeshPtr mesh = std::make_shared<Mesh>();
        mesh->setAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0, buf0);
        mesh->setAttribute(1, 3, GL_FLOAT, GL_FALSE, 0, 0, buf1);
        mesh->setAttribute(2, 2, GL_FLOAT, GL_FALSE, 0, 0, buf2);
        mesh->setPrimitiveType(GL_TRIANGLES);
        mesh->setVertexCount(vertices.size());

        aiReleaseImport(assimpScene);

        return mesh;
    }
}

/**
Инстансинг
*/
class SampleApplication : public Application
{
public:
    MeshPtr _teapot;
    MeshPtr _teapotArray;
    MeshPtr _teapotDivisor;
        
    std::vector<ShaderProgramPtr> _shaders;
    int _currentIndex = 0;

    //Переменные для управления положением одного источника света
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;

    TexturePtr _brickTex;
    TexturePtr _bufferTex;

    GLuint _sampler;
    
    const unsigned int K = 500; //Количество инстансов

    std::vector<glm::vec3> _positions;
    
    SampleApplication() :
        Application(),
        _currentIndex(0)
    {
    }

    void makeScene() override
    {
        Application::makeScene();

        //=========================================================
        //Создание и загрузка мешей

        srand((int)(glfwGetTime() * 1000));

        float size = 50.0f;
        for (int i = 0; i < K; i++)
        {
            _positions.push_back(glm::vec3(frand() * size - 0.5 * size, frand() * size - 0.5 * size, 0.0));
        }

        //----------------------------

        _teapot = loadFromFile("models/teapot.obj");
        _teapotArray = loadFromFileArray("models/teapot.obj", _positions);

        //----------------------------

        _teapotDivisor = loadFromFile("models/teapot.obj");

        DataBufferPtr buf = std::make_shared<DataBuffer>(GL_ARRAY_BUFFER);
        buf->setData(_positions.size() * sizeof(float) * 3, _positions.data());

        _teapotDivisor->setAttribute(3, 3, GL_FLOAT, GL_FALSE, 0, 0, buf);
        _teapotDivisor->setAttributeDivisor(3, 1);

        //=========================================================
        //Инициализация шейдеров

        _shaders.resize(6);

        _shaders[0] = std::make_shared<ShaderProgram>();
        _shaders[0]->createProgram("shaders/common.vert", "shaders/common.frag");

        _shaders[1] = std::make_shared<ShaderProgram>();
        _shaders[1]->createProgram("shaders/common.vert", "shaders/common.frag");
        
        _shaders[2] = std::make_shared<ShaderProgram>();
        _shaders[2]->createProgram("shaders10/instancingNoMatrix.vert", "shaders/common.frag");

        _shaders[3] = std::make_shared<ShaderProgram>();
        _shaders[3]->createProgram("shaders10/instancingUniform.vert", "shaders/common.frag");

        _shaders[4] = std::make_shared<ShaderProgram>();
        _shaders[4]->createProgram("shaders10/instancingTexture.vert", "shaders/common.frag");

        _shaders[5] = std::make_shared<ShaderProgram>();
        _shaders[5]->createProgram("shaders10/instancingDivisor.vert", "shaders/common.frag");

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

        _bufferTex = std::make_shared<Texture>(GL_TEXTURE_BUFFER);
        _bufferTex->bind();
        buf->attachToTexture(GL_RGB32F_ARB);
        _bufferTex->unbind();

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

            ImGui::RadioButton("No instancing", &_currentIndex, 0);
            ImGui::RadioButton("Static instancing", &_currentIndex, 1);
            ImGui::RadioButton("No matrix instancing", &_currentIndex, 2);
            ImGui::RadioButton("Uniform instancing", &_currentIndex, 3);
            ImGui::RadioButton("Texture instancing", &_currentIndex, 4);
            ImGui::RadioButton("Divisor instancing", &_currentIndex, 5);
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
                _currentIndex = 0;
            }
            else if (key == GLFW_KEY_2)
            {
                _currentIndex = 1;
            }
            else if (key == GLFW_KEY_3)
            {
                _currentIndex = 2;
            }
            else if (key == GLFW_KEY_4)
            {
                _currentIndex = 3;
            }
            else if (key == GLFW_KEY_5)
            {
                _currentIndex = 4;
            }
            else if (key == GLFW_KEY_6)
            {
                _currentIndex = 5;
            }
        }
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

        if (_currentIndex == 0 || _currentIndex == 1)
        {
            drawScene(_shaders[_currentIndex]);
        }

        if (_currentIndex == 2)
        {
            drawNoMatrixInstancedScene(_shaders[_currentIndex]);
        }

        if (_currentIndex == 3)
        {
            drawUniformInstancedScene(_shaders[_currentIndex]);
        }

        if (_currentIndex == 4)
        {
            drawTextureInstancedScene(_shaders[_currentIndex]);
        }

        if (_currentIndex == 5)
        {
            drawDivisorInstancedScene(_shaders[_currentIndex]);
        }

        //Отсоединяем сэмплер и шейдерную программу
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawScene(const ShaderProgramPtr& shader)
    {
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

        if (_currentIndex == 0)
        {
            for (unsigned int i = 0; i < _positions.size(); i++)
            {
                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), _positions[i]);

                shader->setMat4Uniform("modelMatrix", modelMatrix);
                shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * modelMatrix))));

                _teapot->draw();
            }
        }

        if (_currentIndex == 1)
        {
            glm::mat4 modelMatrix = glm::mat4(1.0);

            shader->setMat4Uniform("modelMatrix", modelMatrix);
            shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * modelMatrix))));

            _teapotArray->draw();
        }
    }

    void drawNoMatrixInstancedScene(const ShaderProgramPtr& shader)
    {
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

        glm::mat4 modelMatrix = glm::mat4(1.0);
        shader->setMat4Uniform("modelMatrix", modelMatrix);
        shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * modelMatrix))));

        _teapot->drawInstanced(_positions.size());
    }

    void drawUniformInstancedScene(const ShaderProgramPtr& shader)
    {
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

        shader->setVec3UniformArray("positions", _positions);

        glm::mat4 modelMatrix = glm::mat4(1.0);
        shader->setMat4Uniform("modelMatrix", modelMatrix);
        shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * modelMatrix))));

        _teapot->drawInstanced(_positions.size());
    }

    void drawTextureInstancedScene(const ShaderProgramPtr& shader)
    {
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

        glActiveTexture(GL_TEXTURE1);  //текстурный юнит 1
        _bufferTex->bind();
        shader->setIntUniform("texBuf", 1);

        glm::mat4 modelMatrix = glm::mat4(1.0);
        shader->setMat4Uniform("modelMatrix", modelMatrix);
        shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * modelMatrix))));

        _teapot->drawInstanced(_positions.size());
    }

    void drawDivisorInstancedScene(const ShaderProgramPtr& shader)
    {
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

        glm::mat4 modelMatrix = glm::mat4(1.0);
        shader->setMat4Uniform("modelMatrix", modelMatrix);
        shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * modelMatrix))));

        _teapotDivisor->drawInstanced(_positions.size());
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}