#include <Application.hpp>
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

/**
Инстансинг
*/
class SampleApplication : public Application
{
public:
    MeshPtr _teapot;
    MeshPtr _teapotArray;
    MeshPtr _teapotDivisor;

    //Идентификатор шейдерной программы
    ShaderProgramPtr _commonShader;
    
    ShaderProgramPtr _instancingNoMatrixShader;
    ShaderProgramPtr _instancingUniformShader;
    ShaderProgramPtr _instancingTextureShader;
    ShaderProgramPtr _instancingDivisorShader;

    //Переменные для управления положением одного источника света
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;
    CameraInfo _lightCamera;

    TexturePtr _brickTex;
    TexturePtr _bufferTex;

    GLuint _sampler;
    GLuint _cubeTexSampler;
    GLuint _depthSampler;

    bool _noInstancing;
    bool _staticInstancing;
    bool _noMatrixInstancing;
    bool _uniformInstancing;
    bool _textureInstancing;
    bool _divisorInstancing;

    float _oldTime;
    float _deltaTime;
    float _fps;
    std::deque<float> _fpsData;

    std::vector<glm::vec3> _positions;

    void makeScene() override
    {
        Application::makeScene();

        _oldTime = 0.0;
        _deltaTime = 0.0;
        _fps = 0.0;

        _noInstancing = true;
        _staticInstancing = false;
        _noMatrixInstancing = false;
        _uniformInstancing = false;
        _textureInstancing = false;
        _divisorInstancing = false;

        //=========================================================
        //Создание и загрузка мешей

        srand((int)(glfwGetTime() * 1000));

        float size = 50.0f;
        for (int i = 0; i < 500; i++)
        {
            _positions.push_back(glm::vec3(frand() * size - 0.5 * size, frand() * size - 0.5 * size, 0.0));
        }

        _teapot = loadFromFile("models/teapot.obj");

        _teapotArray = loadFromFileArray("models/teapot.obj", _positions);

        _teapotDivisor = loadFromFile("models/teapot.obj");

        DataBufferPtr buf = std::make_shared<DataBuffer>(GL_ARRAY_BUFFER);
        buf->setData(_positions.size() * sizeof(float) * 3, _positions.data());

        _teapotDivisor->setAttribute(3, 3, GL_FLOAT, GL_FALSE, 0, 0, buf);
        _teapotDivisor->setAttributeDivisor(3, 1);

        //=========================================================
        //Инициализация шейдеров

        _commonShader = std::make_shared<ShaderProgram>();
        _commonShader->createProgram("shaders6/common.vert", "shaders6/common.frag");
        
        _instancingNoMatrixShader = std::make_shared<ShaderProgram>();
        _instancingNoMatrixShader->createProgram("shaders10/instancingNoMatrix.vert", "shaders6/common.frag");

        _instancingUniformShader = std::make_shared<ShaderProgram>();
        _instancingUniformShader->createProgram("shaders10/instancingUniform.vert", "shaders6/common.frag");

        _instancingTextureShader = std::make_shared<ShaderProgram>();
        _instancingTextureShader->createProgram("shaders10/instancingTexture.vert", "shaders6/common.frag");

        _instancingDivisorShader = std::make_shared<ShaderProgram>();
        _instancingDivisorShader->createProgram("shaders10/instancingDivisor.vert", "shaders6/common.frag");

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

    void initGUI() override
    {
        Application::initGUI();

        TwAddVarRO(_bar, "FPS", TW_TYPE_FLOAT, &_fps, "");
        TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
        TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
        TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
        TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_light.ambient, "group=Light label='ambient'");
        TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_light.diffuse, "group=Light label='diffuse'");
        TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_light.specular, "group=Light label='specular'");
        TwAddVarRW(_bar, "No instancing", TW_TYPE_BOOLCPP, &_noInstancing, "");
        TwAddVarRW(_bar, "Static instancing", TW_TYPE_BOOLCPP, &_staticInstancing, "");
        TwAddVarRW(_bar, "No Matrix instancing", TW_TYPE_BOOLCPP, &_noMatrixInstancing, "");
        TwAddVarRW(_bar, "Uniform instancing", TW_TYPE_BOOLCPP, &_uniformInstancing, "");
        TwAddVarRW(_bar, "Texture instancing", TW_TYPE_BOOLCPP, &_textureInstancing, "");
        TwAddVarRW(_bar, "Divisor instancing", TW_TYPE_BOOLCPP, &_divisorInstancing, "");
    }

    virtual void handleKey(int key, int scancode, int action, int mods)
    {
        Application::handleKey(key, scancode, action, mods);

        if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_1)
            {
                _noInstancing = !_noInstancing;
            }
            else if (key == GLFW_KEY_2)
            {
                _staticInstancing = !_staticInstancing;
            }
            else if (key == GLFW_KEY_3)
            {
                _noMatrixInstancing = !_noMatrixInstancing;
            }
            else if (key == GLFW_KEY_4)
            {
                _uniformInstancing = !_uniformInstancing;
            }
            else if (key == GLFW_KEY_5)
            {
                _textureInstancing = !_textureInstancing;
            }
            else if (key == GLFW_KEY_6)
            {
                _divisorInstancing = !_divisorInstancing;
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

    void draw() override
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (_noInstancing || _staticInstancing)
        {
            drawScene(_commonShader);
        }

        if (_noMatrixInstancing)
        {
            drawNoMatrixInstancedScene(_instancingNoMatrixShader);
        }

        if (_uniformInstancing)
        {
            drawUniformInstancedScene(_instancingUniformShader);
        }

        if (_textureInstancing)
        {
            drawTextureInstancedScene(_instancingTextureShader);
        }

        if (_divisorInstancing)
        {
            drawDivisorInstancedScene(_instancingDivisorShader);
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
        _brickTex->bind();
        glBindSampler(0, _sampler);
        shader->setIntUniform("diffuseTex", 0);

        if (_noInstancing)
        {
            for (unsigned int i = 0; i < _positions.size(); i++)
            {
                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), _positions[i]);

                shader->setMat4Uniform("modelMatrix", modelMatrix);
                shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * modelMatrix))));

                _teapot->draw();
            }
        }

        if (_staticInstancing)
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
        _brickTex->bind();
        glBindSampler(0, _sampler);
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
        _brickTex->bind();
        glBindSampler(0, _sampler);
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
        _brickTex->bind();
        glBindSampler(0, _sampler);
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
        _brickTex->bind();
        glBindSampler(0, _sampler);
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