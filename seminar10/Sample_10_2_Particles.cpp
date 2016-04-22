#include <Application.hpp>
#include <LightInfo.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

#include <iostream>
#include <sstream>
#include <vector>
#include <deque>

namespace
{
    const int numParticles = 1000;
    const float emitterSize = 0.25;
    const float lifeTime = 5.0;

    struct Particle
    {
        glm::vec3 position;
        glm::vec3 velocity;
        float startTime;

        Particle() : startTime(0.0) {}
    };

    float frand()
    {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
}

/**
������ � �������� ������ �� ������ Point Sprite
*/
class SampleApplication : public Application
{
public:    
    MeshPtr _ground;

    ShaderProgramPtr _groundShader;
    ShaderProgramPtr _particleShader;

    TexturePtr _grassTex;
    TexturePtr _particleTex;

    GLuint _sampler;
    GLuint _grassSampler;
    
    float _oldTime;
    
    std::vector<Particle> _particles;
    std::vector<glm::vec3> _particlePositions;
    std::vector<float> _particleTimes;

    GLuint _particlePosVbo;
    GLuint _particleTimeVbo;
    GLuint _particleVao;

    bool _wind;

    SampleApplication() :
        Application(),
        _oldTime(0.0),
        _wind(false)
    {
    }

    void makeScene() override
    {
        Application::makeScene();

        GLint range[2];
        glGetIntegerv(GL_POINT_SIZE_RANGE, range);

        std::cout << "Point size range " << range[0] << " " << range[1] << std::endl;       

        //=========================================================
        //�������� � �������� �����		

        _ground = makeGroundPlane(10.0f, 5.0f);

        //=========================================================
        //������������� ��������

        _groundShader = std::make_shared<ShaderProgram>();
        _groundShader->createProgram("shaders10/ground.vert", "shaders10/ground.frag");

        _particleShader = std::make_shared<ShaderProgram>();
        _particleShader->createProgram("shaders10/particle.vert", "shaders10/particle.frag");

        //=========================================================
        //�������� � �������� �������
        _particleTex = loadTexture("images/smoke.png", false, true);
        _grassTex = loadTexture("images/grass.jpg", false, false);

        //=========================================================
        //������������� ��������, �������, ������� ������ ��������� ������ �� ��������
        glGenSamplers(1, &_sampler);
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenSamplers(1, &_grassSampler);
        glSamplerParameteri(_grassSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_grassSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(_grassSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_grassSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

        //=========================================================

        srand((int)(glfwGetTime() * 1000));

        _particlePositions.resize(numParticles, glm::vec3());
        _particleTimes.resize(numParticles, 0.0f);
        
        _particlePosVbo = 0;
        glGenBuffers(1, &_particlePosVbo);
        glBindBuffer(GL_ARRAY_BUFFER, _particlePosVbo);
        glBufferData(GL_ARRAY_BUFFER, _particlePositions.size() * sizeof(float) * 3, _particlePositions.data(), GL_STREAM_DRAW);

        _particleTimeVbo = 0;
        glGenBuffers(1, &_particleTimeVbo);
        glBindBuffer(GL_ARRAY_BUFFER, _particleTimeVbo);
        glBufferData(GL_ARRAY_BUFFER, _particleTimes.size() * sizeof(float), _particleTimes.data(), GL_STREAM_DRAW);

        _particleVao = 0;
        glGenVertexArrays(1, &_particleVao);
        glBindVertexArray(_particleVao);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, _particlePosVbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, _particleTimeVbo);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);

        glBindVertexArray(0);
    }

    void updateGUI() override
    {
        Application::updateGUI();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("MIPT OpenGL Sample", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);
                        
            ImGui::Checkbox("Wind", &_wind);
        }
        ImGui::End();
    }

    Particle makeNewParticle()
    {
        float r = frand() * emitterSize;
        float theta = frand() * 0.1f;
        float phi = frand() * 2.0f * glm::pi<float>();

        Particle p;
        p.startTime = static_cast<float>(glfwGetTime());
        p.position = glm::vec3(cos(phi) * r, sin(phi) * r, 0.0);
        p.velocity = glm::vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta)) * 0.5f;

        return p;
    }

    void updateParticles(float deltaTime)
    {
        float curTime = static_cast<float>(glfwGetTime());
        float animationDeltaTime = glm::min(deltaTime, 0.03f);

        if (_particles.size() < numParticles)
        {
            //�� �����, ����� ������� ���������� �� ��� �����, � ���������� �� ���������� ����� ������� ��������� lifeTime
            //����� ������ ������������� ��� ������ �������

            int planningCount = static_cast<int>(numParticles * glfwGetTime() / lifeTime);
            int addParticlesCount = glm::min<int>(numParticles - _particles.size(), planningCount - _particles.size());

            for (int i = 0; i < addParticlesCount; i++)
            {
                Particle p = makeNewParticle();
                p.startTime = frand() * lifeTime; //��������� ������� �������, ����� ������� ���� ������������ ���� ����� ����������
                _particles.push_back(p);
            }
        }
                
        for (unsigned int i = 0; i < _particles.size(); i++)
        {
            //_particles[i].velocity += glm::vec3(0.0, 0.0, -1.0) * animationDeltaTime; //����������
            _particles[i].position += _particles[i].velocity * animationDeltaTime;

            if (_wind)
            {
                _particles[i].position += glm::vec3(0.0, _particles[i].position.z * 0.1, 0.0) * animationDeltaTime;
            }

            if (curTime - _particles[i].startTime > lifeTime)
            {
                _particles[i] = makeNewParticle();
            }

            if (_particles[i].position.z < 0.0)
            {
                //_particles[i] = makeNewParticle();
            }

            _particlePositions[i] = _particles[i].position;
            _particleTimes[i] = _particles[i].startTime;
        }

        glBindBuffer(GL_ARRAY_BUFFER, _particlePosVbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, _particlePositions.size() * sizeof(float) * 3, _particlePositions.data());

        glBindBuffer(GL_ARRAY_BUFFER, _particleTimeVbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, _particleTimes.size() * sizeof(float), _particleTimes.data());
    }

    void update()
    {
        Application::update();

        float time = static_cast<float>(glfwGetTime());
        float deltaTime = time - _oldTime;
        _oldTime = time;
        
        updateParticles(deltaTime);
    }

    void draw() override
    {
        //�������� ������� ������� ������ � ��������� �������
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //������� ������ ����� � ������� �� ����������� ���������� ����������� �����
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //-------------------------

        _groundShader->use();

        _groundShader->setMat4Uniform("modelMatrix", glm::mat4(1.0f));
        _groundShader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _groundShader->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0        
        glBindSampler(0, _grassSampler);
        _grassTex->bind();
        _groundShader->setIntUniform("tex", 0);

        _ground->draw();

        //-------------------------

        drawParticles(_particleShader);

        //-------------------------

        //����������� ������� � ��������� ���������
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawParticles(const ShaderProgramPtr& shader)
    {
        shader->use();

        //��������� �� ���������� �������� �������-����������
        shader->setMat4Uniform("modelMatrix", glm::mat4(1.0f));
        shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);
        shader->setFloatUniform("time", static_cast<float>(glfwGetTime()));

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0        
        glBindSampler(0, _sampler);
        _particleTex->bind();
        shader->setIntUniform("tex", 0);

        glEnable(GL_PROGRAM_POINT_SIZE);        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDepthMask(false);

        glBindVertexArray(_particleVao); //���������� VertexArray
        glDrawArrays(GL_POINTS, 0, _particles.size()); //������		

        glDepthMask(true);

        glDisable(GL_BLEND);                
        glDisable(GL_PROGRAM_POINT_SIZE);
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}