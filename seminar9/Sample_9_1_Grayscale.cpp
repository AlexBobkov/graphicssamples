#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Framebuffer.h"

#include <iostream>
#include <sstream>
#include <vector>

struct LightInfo
{
	glm::vec3 position; //����� ����� ������� ���������� � ������� ������� ���������, � ��� ����������� � �������-���������� �������������� � ������� ����������� ������
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

float frand()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void getColorFromLinearPalette(float value, float& r, float& g, float& b);

/**
������ � ������
*/
class SampleApplication : public Application
{
public:
	Mesh cube;
	Mesh sphere;
	Mesh bunny;
	Mesh ground;
	Mesh backgroundCube;

	Mesh quad;

	Mesh marker; //��� - ������ ��� ��������� �����

	//������������� ��������� ���������
	ShaderProgram _commonShader;
	ShaderProgram _markerShader;
	ShaderProgram _skyboxShader;
	ShaderProgram _quadDepthShader;
    ShaderProgram _quadColorShader;
    ShaderProgram _renderToGBufferShader;
    ShaderProgram _renderDeferredShader;

	//���������� ��� ���������� ���������� ������ ��������� �����
	float _lr;
	float _phi;
	float _theta;
		
	LightInfo _light;
    
	GLuint _worldTexId;
	GLuint _brickTexId;
	GLuint _grassTexId;
	GLuint _chessTexId;
	GLuint _myTexId;
	GLuint _cubeTexId;    

	GLuint _sampler;
	GLuint _cubeTexSampler;
    
    bool _showDebugQuads;

    Framebuffer _gbufferFB;
    GLuint _depthTexId;
    GLuint _normalsTexId;
    GLuint _diffuseTexId;

    //������ ������� ������
    int _oldWidth;
    int _oldHeight;
    
    void initFramebuffers()
    {
        _gbufferFB.create();
        _gbufferFB.bind();
        _gbufferFB.setSize(1024, 1024);

        _normalsTexId = _gbufferFB.addBuffer(GL_RGB16F, GL_COLOR_ATTACHMENT0);
        _diffuseTexId = _gbufferFB.addBuffer(GL_RGB8, GL_COLOR_ATTACHMENT1);
        _depthTexId = _gbufferFB.addBuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT);

        _gbufferFB.initDrawBuffers();

        if (!_gbufferFB.valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        _gbufferFB.unbind();
    }

	virtual void makeScene()
	{
		Application::makeScene();   

        _showDebugQuads = false;

		//=========================================================
		//�������� � �������� �����		

		cube.makeCube(0.5);
		cube.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));

		sphere.makeSphere(0.5, 100);
		sphere.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));

		bunny.loadFromFile("models/bunny.obj");
		bunny.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		ground.makeGroundPlane(5.0f, 2.0f);

		marker.makeSphere(0.1);

		backgroundCube.makeCube(10.0f);

		quad.makeScreenAlignedQuad();

		//=========================================================
		//������������� ��������

		_commonShader.createProgram("shaders6/common.vert", "shaders6/common.frag");
		_markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");
		_skyboxShader.createProgram("shaders6/skybox.vert", "shaders6/skybox.frag");
		_quadDepthShader.createProgram("shaders7/quadDepth.vert", "shaders7/quadDepth.frag");
        _quadColorShader.createProgram("shaders7/quadColor.vert", "shaders7/quadColor.frag");

        _renderToGBufferShader.createProgram("shaders8/togbuffer.vert", "shaders8/togbuffer.frag");
        _renderDeferredShader.createProgram("shaders9/deferred.vert", "shaders9/deferred.frag");

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
		_worldTexId = Texture::loadTexture("images/earth_global.jpg");
		_brickTexId = Texture::loadTexture("images/brick.jpg");
		_grassTexId = Texture::loadTexture("images/grass.jpg");
		_chessTexId = Texture::loadTextureWithMipmaps("images/chess.dds");
		_myTexId = Texture::makeProceduralTexture();
		_cubeTexId = Texture::loadCubeTexture("images/cube");

		//=========================================================
		//������������� ��������, �������, ������� ������ ��������� ������ �� ��������
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

        //=========================================================
        glfwGetFramebufferSize(_window, &_oldWidth, &_oldHeight);

        //=========================================================
        //������������� ����������� ��� ������� ������� �����

        initFramebuffers();
	}

	virtual void initGUI()
	{
		Application::initGUI();

		TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
		TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
		TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
		TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_light.ambient, "group=Light label='ambient'");
		TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_light.diffuse, "group=Light label='diffuse'");
		TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_light.specular, "group=Light label='specular'");
	}

    virtual void handleKey(int key, int scancode, int action, int mods)
    {
        Application::handleKey(key, scancode, action, mods);

        if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_Z)
            {
                _showDebugQuads = !_showDebugQuads;
            }
        }
    }

    void update()
    {
        Application::update();

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;

        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);
        if (width != _oldWidth || height != _oldHeight)
        {
            _gbufferFB.resize(width, height);

            _oldWidth = width;
            _oldHeight = height;
        }
    }

    virtual void draw()
    {
        drawToGBuffer(_camera);
        drawToScreen(_renderDeferredShader, _camera);

        if (_showDebugQuads)
        {
            drawDebug();
        }
    }

    void drawToGBuffer(const CameraInfo& camera)
    {
        //=========== ������� ���������� ���������� � �������� � �������� ==========

        _gbufferFB.bind();

        glViewport(0, 0, _gbufferFB.width(), _gbufferFB.height());                
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _renderToGBufferShader.use();
        _renderToGBufferShader.setMat4Uniform("viewMatrix", camera.viewMatrix);
        _renderToGBufferShader.setMat4Uniform("projectionMatrix", camera.projMatrix);

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
        glBindTexture(GL_TEXTURE_2D, _brickTexId);
        glBindSampler(0, _sampler);
        _renderToGBufferShader.setIntUniform("diffuseTex", 0);

        drawScene(_renderToGBufferShader, camera);

        glUseProgram(0); //��������� ������

        _gbufferFB.unbind(); //��������� ����������
    }

    void drawToScreen(const ShaderProgram& shader, const CameraInfo& camera)
	{
		//�������� ������� ������� ������ � ��������� �������
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);

		glViewport(0, 0, width, height);
        
		//������� ������ ����� � ������� �� ����������� ���������� ����������� �����
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("projMatrixInverse", glm::inverse(camera.projMatrix));

        glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_light.position, 1.0));

        shader.setVec3Uniform("light.pos", lightPosCamSpace); //�������� ��������� ��� � ������� ����������� ������
        shader.setVec3Uniform("light.La", _light.ambient);
        shader.setVec3Uniform("light.Ld", _light.diffuse);
        shader.setVec3Uniform("light.Ls", _light.specular);
        		
        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
        glBindTexture(GL_TEXTURE_2D, _normalsTexId);
        glBindSampler(0, _sampler);
        shader.setIntUniform("normalsTex", 0);

        glActiveTexture(GL_TEXTURE1);  //���������� ���� 1
        glBindTexture(GL_TEXTURE_2D, _diffuseTexId);
        glBindSampler(1, _sampler);
        shader.setIntUniform("diffuseTex", 1);

        glActiveTexture(GL_TEXTURE2);  //���������� ���� 2
        glBindTexture(GL_TEXTURE_2D, _depthTexId);
        glBindSampler(2, _sampler);
        shader.setIntUniform("depthTex", 2);
        
        quad.draw(); //main light

        //����������� ������� � ��������� ���������
		glBindSampler(0, 0);
		glUseProgram(0);
	}

    void drawScene(const ShaderProgram& shader, const CameraInfo& camera)
    {
        shader.setMat4Uniform("modelMatrix", cube.modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * cube.modelMatrix()))));

        cube.draw();

        shader.setMat4Uniform("modelMatrix", sphere.modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * sphere.modelMatrix()))));

        sphere.draw();
                
        shader.setMat4Uniform("modelMatrix", ground.modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * ground.modelMatrix()))));

        ground.draw();

        shader.setMat4Uniform("modelMatrix", bunny.modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * bunny.modelMatrix()))));
        
        bunny.draw();
    }

    void drawDebug()
    {
        glClear(GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, 500, 500);

        _quadDepthShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _depthTexId);
        glBindSampler(0, _sampler);
        _quadDepthShader.setIntUniform("tex", 0);

        quad.draw();

        glViewport(500, 0, 500, 500);

        _quadColorShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _normalsTexId);
        glBindSampler(0, _sampler);
        _quadColorShader.setIntUniform("tex", 0);

        quad.draw();

        glViewport(1000, 0, 500, 500);

        _quadColorShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _diffuseTexId);
        glBindSampler(0, _sampler);
        _quadColorShader.setIntUniform("tex", 0);

        quad.draw();

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