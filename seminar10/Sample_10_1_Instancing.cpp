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

    /**
    ��������� 3�-������, ���������� � � ������ positions.size() ���, ������� ���������� � ������� ������� ���������
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
����������
*/
class SampleApplication : public Application
{
public:
    enum Mode : int
    {
        NO_INSTANCING,
        BATCH,
        NO_MATRIX,
        UNIFORM,
        UBO,
        SSBO,
        TEXTURE,
        DIVISOR        
    };

    MeshPtr _teapot;
    MeshPtr _teapotArray;
    MeshPtr _teapotDivisor;

    TexturePtr _brickTex;
    GLuint _sampler;

    //------------------------
        
    std::vector<ShaderProgramPtr> _shaders;
    Mode _currentMode;

    //------------------------

    //���������� ��� ���������� ���������� ������ ��������� �����
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;

    //------------------------

    const unsigned int K = 1000; //���������� ���������

    std::vector<glm::vec3> _positionsVec3;
    std::vector<glm::vec4> _positionsVec4;

    DataBufferPtr _bufVec3;
    DataBufferPtr _bufVec4;
    
    TexturePtr _bufferTex;

    //------------------------
    
    SampleApplication() :
        Application(),
        _currentMode(NO_INSTANCING)
    {
    }

    void makeScene() override
    {
        Application::makeScene();

        //������������ ���������� �������� ���������, ���������� � ���������� vec4
        GLint maxVertexAttributes;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttributes);

        //������������ ������ ���� �������-���������, ���������� � ���������� vec4
        GLint maxVertexUniformVectors;
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertexUniformVectors);

        //������������ ������ �������-����� � ������
        GLint maxUniformBlockSize;
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBlockSize);

        //������������ ������ ShaderStorage-����� � ������
        GLint maxShaderStorageBlockSize;
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxShaderStorageBlockSize);

        //������������ ���������� �������� � ���������� ������
        GLint maxTextureBufferSize;
        glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &maxTextureBufferSize);

        std::cout << "GL_MAX_VERTEX_ATTRIBS " << maxVertexAttributes << std::endl;
        std::cout << "GL_MAX_VERTEX_UNIFORM_VECTORS " << maxVertexUniformVectors << std::endl;
        std::cout << "GL_MAX_UNIFORM_BLOCK_SIZE " << maxUniformBlockSize << std::endl;
        std::cout << "GL_MAX_SHADER_STORAGE_BLOCK_SIZE " << maxShaderStorageBlockSize << std::endl;            
        std::cout << "GL_MAX_TEXTURE_BUFFER_SIZE " << maxTextureBufferSize << std::endl;

        //=========================================================
        //�������������� K ��������� ������� ��� K �����������
        //��������� �������� ������� ����������� ������ �� vec4, � ��������� ���

        srand((int)(glfwGetTime() * 1000));

        float size = 50.0f;
        for (unsigned int i = 0; i < K; i++)
        {
            _positionsVec3.push_back(glm::vec3(frand() * size - 0.5 * size, frand() * size - 0.5 * size, 0.0));
            _positionsVec4.push_back(glm::vec4(_positionsVec3.back(), 0.0));
        }

        //������� ������ ��� ������������ (_bufVec3) � � ������������� (_bufVec4)

        _bufVec3 = std::make_shared<DataBuffer>(GL_ARRAY_BUFFER);
        _bufVec3->setData(_positionsVec3.size() * sizeof(float) * 3, _positionsVec3.data());

        _bufVec4 = std::make_shared<DataBuffer>(GL_ARRAY_BUFFER);
        _bufVec4->setData(_positionsVec4.size() * sizeof(float) * 4, _positionsVec4.data());

        //----------------------------

        //����������� SSBO � 0� ����� �������� (��������� ����� � �������������)
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _bufVec4->id());

        //----------------------------

        //����������� UBO � 0� ����� �������� (��������� ����� � �������������)
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _bufVec4->id());

        //----------------------------

        //������� ���������� ����� � ����������� � ���� ����� ��� ������������
        _bufferTex = std::make_shared<Texture>(GL_TEXTURE_BUFFER);
        _bufferTex->bind();
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F_ARB, _bufVec3->id());
        _bufferTex->unbind();

        //----------------------------

        _teapot = loadFromFile("models/teapot.obj");
        _teapotArray = loadFromFileArray("models/teapot.obj", _positionsVec3);

        //----------------------------

        _teapotDivisor = loadFromFile("models/teapot.obj");
                
        //���������� ����� ��� ������������ � �������� ���������� ��������
        _teapotDivisor->setAttribute(3, 3, GL_FLOAT, GL_FALSE, 0, 0, _bufVec3);
        _teapotDivisor->setAttributeDivisor(3, 1);        

        //=========================================================
        //������������� ��������

        _shaders.resize(8);

        _shaders[NO_INSTANCING] = std::make_shared<ShaderProgram>();
        _shaders[NO_INSTANCING]->createProgram("shaders/common.vert", "shaders/common.frag");

        _shaders[BATCH] = std::make_shared<ShaderProgram>();
        _shaders[BATCH]->createProgram("shaders/common.vert", "shaders/common.frag");
        
        _shaders[NO_MATRIX] = std::make_shared<ShaderProgram>();
        _shaders[NO_MATRIX]->createProgram("shaders10/instancingNoMatrix.vert", "shaders/common.frag");

        _shaders[UNIFORM] = std::make_shared<ShaderProgram>();
        _shaders[UNIFORM]->createProgram("shaders10/instancingUniform.vert", "shaders/common.frag");

        _shaders[UBO] = std::make_shared<ShaderProgram>();
        _shaders[UBO]->createProgram("shaders10/instancingUBO.vert", "shaders/common.frag");

        _shaders[SSBO] = std::make_shared<ShaderProgram>();
        _shaders[SSBO]->createProgram("shaders10/instancingSSBO.vert", "shaders/common.frag");

        _shaders[TEXTURE] = std::make_shared<ShaderProgram>();
        _shaders[TEXTURE]->createProgram("shaders10/instancingTexture.vert", "shaders/common.frag");

        _shaders[DIVISOR] = std::make_shared<ShaderProgram>();
        _shaders[DIVISOR]->createProgram("shaders10/instancingDivisor.vert", "shaders/common.frag");

        //=========================================================
        //������������� �������� ���������� ��������
        _lr = 10.0;
        _phi = 0.0f;
        _theta = 0.48f;

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _light.ambient = glm::vec3(0.2, 0.2, 0.2);
        _light.diffuse = glm::vec3(0.8, 0.8, 0.8);
        _light.specular = glm::vec3(1.0, 1.0, 1.0);

        //=========================================================
        //�������� � �������� �������
        _brickTex = loadTexture("images/brick.jpg");
                
        //=========================================================
        //������������� ��������, �������, ������� ������ ��������� ������ �� ��������
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

            ImGui::RadioButton("No instancing", reinterpret_cast<int*>(&_currentMode), NO_INSTANCING);
            ImGui::RadioButton("Batch", reinterpret_cast<int*>(&_currentMode), BATCH);
            ImGui::RadioButton("No matrix", reinterpret_cast<int*>(&_currentMode), NO_MATRIX);
            ImGui::RadioButton("Uniform", reinterpret_cast<int*>(&_currentMode), UNIFORM);
            ImGui::RadioButton("UBO", reinterpret_cast<int*>(&_currentMode), UBO);
            ImGui::RadioButton("SSBO", reinterpret_cast<int*>(&_currentMode), SSBO);
            ImGui::RadioButton("Texture", reinterpret_cast<int*>(&_currentMode), TEXTURE);
            ImGui::RadioButton("Divisor", reinterpret_cast<int*>(&_currentMode), DIVISOR);
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
                _currentMode = NO_INSTANCING;
            }
            else if (key == GLFW_KEY_2)
            {
                _currentMode = BATCH;
            }
            else if (key == GLFW_KEY_3)
            {
                _currentMode = NO_MATRIX;
            }
            else if (key == GLFW_KEY_4)
            {
                _currentMode = UNIFORM;
            }
            else if (key == GLFW_KEY_5)
            {
                _currentMode = UBO;
            }
            else if (key == GLFW_KEY_6)
            {
                _currentMode = SSBO;
            }
            else if (key == GLFW_KEY_7)
            {
                _currentMode = TEXTURE;
            }
            else if (key == GLFW_KEY_8)
            {
                _currentMode = DIVISOR;
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
        //�������� ������� ������� ������ � ��������� �������
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //������� ������ ����� � ������� �� ����������� ���������� ����������� �����
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawScene(_shaders[_currentMode]);

        //����������� ������� � ��������� ���������
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawScene(const ShaderProgramPtr& shader)
    {
        shader->use();

        //��������� �� ���������� �������� �������-����������
        shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        glm::vec3 lightPosCamSpace = glm::vec3(_camera.viewMatrix * glm::vec4(_light.position, 1.0));
        shader->setVec3Uniform("light.pos", lightPosCamSpace); //�������� ��������� ��� � ������� ����������� ������
        shader->setVec3Uniform("light.La", _light.ambient);
        shader->setVec3Uniform("light.Ld", _light.diffuse);
        shader->setVec3Uniform("light.Ls", _light.specular);

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0        
        glBindSampler(0, _sampler);
        _brickTex->bind();
        shader->setIntUniform("diffuseTex", 0);

        if (_currentMode == NO_INSTANCING)
        {
            for (unsigned int i = 0; i < _positionsVec3.size(); i++)
            {
                glm::mat4 modelMatrix = glm::translate(_positionsVec3[i]);

                shader->setMat4Uniform("modelMatrix", modelMatrix);
                shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * modelMatrix))));

                _teapot->draw();
            }
        }
        else if (_currentMode == BATCH)
        {
            shader->setMat4Uniform("modelMatrix", glm::mat4(1.0));
            shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix))));

            _teapotArray->draw();
        }
        else if (_currentMode == DIVISOR)
        {
            shader->setMat4Uniform("modelMatrix", glm::mat4(1.0));
            shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix))));

            _teapotDivisor->drawInstanced(_positionsVec3.size());
        }
        else
        {
            if (_currentMode == UNIFORM)
            {
                shader->setVec3UniformArray("positions", _positionsVec3);
            }
            else if (_currentMode == UBO)
            {
                unsigned int uboIndex = glGetUniformBlockIndex(shader->id(), "Positions");
                glUniformBlockBinding(shader->id(), uboIndex, 0); //0� ����� ��������
            }
            else if (_currentMode == SSBO)
            {
                unsigned int ssboIndex = glGetProgramResourceIndex(shader->id(), GL_SHADER_STORAGE_BLOCK, "Positions");
                glShaderStorageBlockBinding(shader->id(), ssboIndex, 0); //0� ����� ��������
            }
            else if (_currentMode == TEXTURE)
            {
                glActiveTexture(GL_TEXTURE1);
                _bufferTex->bind();
                shader->setIntUniform("texBuf", 1);
            }

            shader->setMat4Uniform("modelMatrix", glm::mat4(1.0));
            shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix))));

            _teapot->drawInstanced(_positionsVec3.size());
        }
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}