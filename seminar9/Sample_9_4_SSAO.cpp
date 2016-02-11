#include "Application.h"
#include "Texture.h"

#include <Framebuffer.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <sstream>
#include <vector>

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

void getColorFromLinearPalette(float value, float& r, float& g, float& b);

/**
Пример эффекта Screen Space Ambient Occlusion
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

	Mesh marker; //Меш - маркер для источника света

	//Идентификатор шейдерной программы
	ShaderProgram _commonShader;
	ShaderProgram _markerShader;
	ShaderProgram _skyboxShader;
	ShaderProgram _quadDepthShader;
    ShaderProgram _quadColorShader;
    ShaderProgram _renderToShadowMapShader;
    ShaderProgram _renderToGBufferShader;
    ShaderProgram _renderDeferredShader;
    ShaderProgram _renderDeferredWithSSAOShader;
    ShaderProgram _gammaShader;
    ShaderProgram _brightShader;
    ShaderProgram _horizBlurShader;
    ShaderProgram _vertBlurShader;
    ShaderProgram _toneMappingShader;
    ShaderProgram _ssaoShader;

	//Переменные для управления положением одного источника света
	float _lr;
	float _phi;
	float _theta;
		
    float _lightIntensity;
	LightInfo _light;
    CameraInfo _lightCamera;
    
	GLuint _worldTexId;
	GLuint _brickTexId;
	GLuint _grassTexId;
	GLuint _chessTexId;
	GLuint _myTexId;
	GLuint _cubeTexId;
    GLuint _rotateTexId;

	GLuint _sampler;
    GLuint _repeatSampler;
	GLuint _cubeTexSampler;
    GLuint _depthSampler;
    
    bool _applyEffect;
    
    bool _showGBufferDebug;
    bool _showShadowDebug;
    bool _showDeferredDebug;
    bool _showHDRDebug;
    bool _showSSAODebug;

    float _exposure; //Параметр алгоритма ToneMapping

    Framebuffer _gbufferFB;
    GLuint _depthTexId;
    GLuint _normalsTexId;
    GLuint _diffuseTexId;

    Framebuffer _shadowFB;
    GLuint _shadowTexId;

    Framebuffer _deferredFB;
    GLuint _deferredTexId;

    Framebuffer _brightFB;
    GLuint _brightTexId;

    Framebuffer _horizBlurFB;
    GLuint _horizBlurTexId;

    Framebuffer _vertBlurFB;
    GLuint _vertBlurTexId;

    Framebuffer _toneMappingFB;
    GLuint _toneMappingTexId;

    Framebuffer _ssaoFB;
    GLuint _ssaoTexId;

    //Старые размеры экрана
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

        //=========================================================

        _shadowFB.create();
        _shadowFB.bind();
        _shadowFB.setSize(1024, 1024);

        _shadowTexId = _shadowFB.addBuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT);

        _shadowFB.initDrawBuffers();

        if (!_shadowFB.valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        _shadowFB.unbind();

        //=========================================================

        _deferredFB.create();
        _deferredFB.bind();
        _deferredFB.setSize(1024, 1024);

        _deferredTexId = _deferredFB.addBuffer(GL_RGB32F, GL_COLOR_ATTACHMENT0);

        _deferredFB.initDrawBuffers();

        if (!_deferredFB.valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        _deferredFB.unbind();

        //=========================================================

        _brightFB.create();
        _brightFB.bind();
        _brightFB.setSize(512, 512); //В 2 раза меньше

        _brightTexId = _brightFB.addBuffer(GL_RGB32F, GL_COLOR_ATTACHMENT0);

        _brightFB.initDrawBuffers();

        if (!_brightFB.valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        _brightFB.unbind();

        //=========================================================

        _horizBlurFB.create();
        _horizBlurFB.bind();
        _horizBlurFB.setSize(512, 512); //В 2 раза меньше

        _horizBlurTexId = _horizBlurFB.addBuffer(GL_RGB32F, GL_COLOR_ATTACHMENT0);

        _horizBlurFB.initDrawBuffers();

        if (!_horizBlurFB.valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        _horizBlurFB.unbind();

        //=========================================================

        _vertBlurFB.create();
        _vertBlurFB.bind();
        _vertBlurFB.setSize(512, 512); //В 2 раза меньше

        _vertBlurTexId = _vertBlurFB.addBuffer(GL_RGB32F, GL_COLOR_ATTACHMENT0);

        _vertBlurFB.initDrawBuffers();

        if (!_vertBlurFB.valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        _vertBlurFB.unbind();

        //=========================================================

        _toneMappingFB.create();
        _toneMappingFB.bind();
        _toneMappingFB.setSize(1024, 1024);

        _toneMappingTexId = _toneMappingFB.addBuffer(GL_RGB8, GL_COLOR_ATTACHMENT0);

        _toneMappingFB.initDrawBuffers();

        if (!_toneMappingFB.valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        _toneMappingFB.unbind();

        //=========================================================

        _ssaoFB.create();
        _ssaoFB.bind();
        _ssaoFB.setSize(1024, 1024);

        _ssaoTexId = _ssaoFB.addBuffer(GL_RGB8, GL_COLOR_ATTACHMENT0);

        _ssaoFB.initDrawBuffers();

        if (!_ssaoFB.valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        _ssaoFB.unbind();
    }

	virtual void makeScene()
	{
		Application::makeScene();   

        _applyEffect = true;
        _showGBufferDebug = false;
        _showShadowDebug = false;
        _showDeferredDebug = false;
        _showHDRDebug = false;
        _showSSAODebug = false;

		//=========================================================
		//Создание и загрузка мешей		

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
		//Инициализация шейдеров

		_commonShader.createProgram("shaders6/common.vert", "shaders6/common.frag");
		_markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");
		_skyboxShader.createProgram("shaders6/skybox.vert", "shaders6/skybox.frag");
		_quadDepthShader.createProgram("shaders7/quadDepth.vert", "shaders7/quadDepth.frag");
        _quadColorShader.createProgram("shaders7/quadColor.vert", "shaders7/quadColor.frag");
        _renderToShadowMapShader.createProgram("shaders8/toshadow.vert", "shaders8/toshadow.frag");
        _renderToGBufferShader.createProgram("shaders8/togbuffer.vert", "shaders8/togbuffer.frag");
        _renderDeferredShader.createProgram("shaders9/deferred.vert", "shaders9/deferred.frag");
        _renderDeferredWithSSAOShader.createProgram("shaders9/deferred.vert", "shaders9/deferredWithSSAO.frag");
        _gammaShader.createProgram("shaders9/quad.vert", "shaders9/gamma.frag");
        _brightShader.createProgram("shaders9/quad.vert", "shaders9/bright.frag");
        _horizBlurShader.createProgram("shaders9/quad.vert", "shaders9/horizblur.frag");
        _vertBlurShader.createProgram("shaders9/quad.vert", "shaders9/vertblur.frag");
        _toneMappingShader.createProgram("shaders9/quad.vert", "shaders9/tonemapping.frag");
        _ssaoShader.createProgram("shaders9/quad.vert", "shaders9/ssao.frag");

		//=========================================================
		//Инициализация значений переменных освщения
		_lr = 10.0;
		_phi = 0.0f;
		_theta = 0.48f;

        _lightIntensity = 1.0f;

		_light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
		_light.ambient = glm::vec3(0.2, 0.2, 0.2);
		_light.diffuse = glm::vec3(0.8, 0.8, 0.8);
		_light.specular = glm::vec3(1.0, 1.0, 1.0);

		//=========================================================
		//Загрузка и создание текстур
		_worldTexId = Texture::loadTexture("images/earth_global.jpg");
		_brickTexId = Texture::loadTexture("images/brick.jpg", true); //sRGB
		_grassTexId = Texture::loadTexture("images/grass.jpg");
		_chessTexId = Texture::loadTextureWithMipmaps("images/chess.dds");
		_myTexId = Texture::makeProceduralTexture();
		_cubeTexId = Texture::loadCubeTexture("images/cube");
        _rotateTexId = Texture::loadTexture("images/rotate.png");

		//=========================================================
		//Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
		glGenSamplers(1, &_sampler);
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenSamplers(1, &_repeatSampler);
        glSamplerParameteri(_repeatSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_repeatSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_repeatSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_repeatSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

        //=========================================================
        glfwGetFramebufferSize(_window, &_oldWidth, &_oldHeight);

        //=========================================================
        //Инициализация фреймбуфера для рендера теневой карты

        initFramebuffers();

        _exposure = 1.0f;
	}

	virtual void initGUI()
	{
		Application::initGUI();

		TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
		TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
		TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
        TwAddVarRW(_bar, "intensity", TW_TYPE_FLOAT, &_lightIntensity, "group=Light step=0.01 min=0.0 max=100.0");
		TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_light.ambient, "group=Light label='ambient'");
		TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_light.diffuse, "group=Light label='diffuse'");
		TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_light.specular, "group=Light label='specular'");
        TwAddVarRO(_bar, "SSAO", TW_TYPE_BOOLCPP, &_applyEffect, "");
        TwAddVarRW(_bar, "Exposure", TW_TYPE_FLOAT, &_exposure, "min=0.01 max=100.0 step=0.01");        
	}

    virtual void handleKey(int key, int scancode, int action, int mods)
    {
        Application::handleKey(key, scancode, action, mods);

        if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_1)
            {
                _applyEffect = !_applyEffect;
            }
            else if (key == GLFW_KEY_Z)
            {
                _showGBufferDebug = !_showGBufferDebug;
            }
            else if (key == GLFW_KEY_X)
            {
                _showShadowDebug = !_showShadowDebug;
            }
            else if (key == GLFW_KEY_C)
            {
                _showDeferredDebug = !_showDeferredDebug;
            }
            else if (key == GLFW_KEY_V)
            {
                _showHDRDebug = !_showHDRDebug;
            }
            else if (key == GLFW_KEY_B)
            {
                _showSSAODebug = !_showSSAODebug;
            }
        }
    }

    void update()
    {
        Application::update();        

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _lightCamera.viewMatrix = glm::lookAt(_light.position, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        _lightCamera.projMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 30.f);

        //Если размер окна изменился, то изменяем размеры фреймбуферов - перевыделяем память под текстуры

        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);
        if (width != _oldWidth || height != _oldHeight)
        {
            _gbufferFB.resize(width, height);
            _deferredFB.resize(width, height);

            _brightFB.resize(width / 2, height / 2);
            _horizBlurFB.resize(width / 2, height / 2);
            _vertBlurFB.resize(width / 2, height / 2);
            _toneMappingFB.resize(width, height);

            _ssaoFB.resize(width, height);

            _oldWidth = width;
            _oldHeight = height;
        }
    }

    virtual void draw()
    {
        //Рендерим геометрию сцены в G-буфер
        drawToGBuffer(_gbufferFB, _renderToGBufferShader, _camera);

        //Рендерим геометрию сцены в теневую карту с позиции источника света
        drawToShadowMap(_shadowFB, _renderToShadowMapShader, _lightCamera);
        
        //Генерируем Ambient Occlusion текстуру
        drawSSAO(_ssaoFB, _ssaoShader, _camera);

        if (_applyEffect)
        {
            //Выполняем отложенное освещение, заодно накладывает тени, а результат записываем в текстуру
            drawDeferred(_deferredFB, _renderDeferredWithSSAOShader, _camera, _lightCamera);
        }
        else
        {
            drawDeferred(_deferredFB, _renderDeferredShader, _camera, _lightCamera);
        }
        
        //Получаем текстуру с яркими областями
        drawProcessTexture(_brightFB, _brightShader, _deferredTexId, _deferredFB.width(), _deferredFB.height());

        //Выполняем размытие текстуры с яркими областями
        drawProcessTexture(_horizBlurFB, _horizBlurShader, _brightTexId, _brightFB.width(), _brightFB.height());
        drawProcessTexture(_vertBlurFB, _vertBlurShader, _horizBlurTexId, _horizBlurFB.width(), _horizBlurFB.height());
                
        drawToneMapping(_toneMappingFB, _toneMappingShader);
        drawToScreen(_gammaShader, _toneMappingTexId);
        
        //Отладочный рендер текстур
        drawDebug();
    }

    void drawToGBuffer(const Framebuffer& fb, const ShaderProgram& shader, const CameraInfo& camera)
    {
        fb.bind();

        glViewport(0, 0, fb.width(), fb.height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("viewMatrix", camera.viewMatrix);
        shader.setMat4Uniform("projectionMatrix", camera.projMatrix);

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
        glBindTexture(GL_TEXTURE_2D, _brickTexId);
        glBindSampler(0, _repeatSampler);
        shader.setIntUniform("diffuseTex", 0);

        drawScene(shader, camera);

        glUseProgram(0); //Отключаем шейдер

        fb.unbind(); //Отключаем фреймбуфер
    }

    void drawToShadowMap(const Framebuffer& fb, const ShaderProgram& shader, const CameraInfo& lightCamera)
    {
        fb.bind();

        glViewport(0, 0, fb.width(), fb.height());
        glClear(GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("viewMatrix", lightCamera.viewMatrix);
        shader.setMat4Uniform("projectionMatrix", lightCamera.projMatrix);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        drawScene(shader, lightCamera);

        glDisable(GL_CULL_FACE);

        glUseProgram(0);

        fb.unbind();
    }

    void drawDeferred(const Framebuffer& fb, const ShaderProgram& shader, const CameraInfo& camera, const CameraInfo& lightCamera)
	{
        fb.bind();

        glViewport(0, 0, fb.width(), fb.height());
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("viewMatrixInverse", glm::inverse(camera.viewMatrix));
        shader.setMat4Uniform("projMatrixInverse", glm::inverse(camera.projMatrix));

        glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_light.position, 1.0));

        shader.setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
        shader.setVec3Uniform("light.La", _light.ambient * _lightIntensity);
        shader.setVec3Uniform("light.Ld", _light.diffuse * _lightIntensity);
        shader.setVec3Uniform("light.Ls", _light.specular * _lightIntensity);

        shader.setMat4Uniform("lightViewMatrix", lightCamera.viewMatrix);
        shader.setMat4Uniform("lightProjectionMatrix", lightCamera.projMatrix);

        glm::mat4 projScaleBiasMatrix = glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5)), glm::vec3(0.5, 0.5, 0.5));
        shader.setMat4Uniform("lightScaleBiasMatrix", projScaleBiasMatrix);
        		
        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
        glBindTexture(GL_TEXTURE_2D, _normalsTexId);
        glBindSampler(0, _sampler);
        shader.setIntUniform("normalsTex", 0);

        glActiveTexture(GL_TEXTURE1);  //текстурный юнит 1
        glBindTexture(GL_TEXTURE_2D, _diffuseTexId);
        glBindSampler(1, _sampler);
        shader.setIntUniform("diffuseTex", 1);

        glActiveTexture(GL_TEXTURE2);  //текстурный юнит 2
        glBindTexture(GL_TEXTURE_2D, _depthTexId);
        glBindSampler(2, _sampler);
        shader.setIntUniform("depthTex", 2);

        glActiveTexture(GL_TEXTURE3);  //текстурный юнит 3
        glBindTexture(GL_TEXTURE_2D, _shadowTexId);
        glBindSampler(3, _depthSampler);
        shader.setIntUniform("shadowTex", 3);

        glActiveTexture(GL_TEXTURE4);  //текстурный юнит 4
        glBindTexture(GL_TEXTURE_2D, _ssaoTexId);
        glBindSampler(4, _sampler);
        shader.setIntUniform("ssaoTex", 4);
        
        quad.draw();

        glUseProgram(0);

        fb.unbind();
	}

    void drawSSAO(const Framebuffer& fb, const ShaderProgram& shader, const CameraInfo& camera)
    {
        fb.bind();

        glViewport(0, 0, fb.width(), fb.height());
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("projMatrix", camera.projMatrix);
        shader.setMat4Uniform("projMatrixInverse", glm::inverse(camera.projMatrix));

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
        glBindTexture(GL_TEXTURE_2D, _depthTexId);
        glBindSampler(0, _sampler);
        shader.setIntUniform("depthTex", 0);

        glActiveTexture(GL_TEXTURE1);  //текстурный юнит 1
        glBindTexture(GL_TEXTURE_2D, _rotateTexId);
        glBindSampler(1, _repeatSampler);
        shader.setIntUniform("rotateTex", 1);

        quad.draw();

        glUseProgram(0);

        fb.unbind();
    }

    void drawProcessTexture(const Framebuffer& fb, const ShaderProgram& shader, GLuint inputTextureId, int inputTexWidth, int inputTexHeight)
    {
        fb.bind();

        glViewport(0, 0, fb.width(), fb.height());
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        shader.setVec2Uniform("texSize", glm::vec2(inputTexWidth, inputTexHeight));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputTextureId);
        glBindSampler(0, _sampler);
        shader.setIntUniform("tex", 0);

        quad.draw();

        glUseProgram(0);

        fb.unbind();
    }

    void drawToneMapping(const Framebuffer& fb, const ShaderProgram& shader)
    {
        fb.bind();

        glViewport(0, 0, fb.width(), fb.height());
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        shader.setFloatUniform("exposure", _exposure);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _deferredTexId);
        glBindSampler(0, _sampler);
        shader.setIntUniform("tex", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _vertBlurTexId);
        glBindSampler(1, _sampler);
        shader.setIntUniform("bloomTex", 1);

        quad.draw();

        glUseProgram(0);

        fb.unbind();
    }

    void drawToScreen(const ShaderProgram& shader, GLuint inputTextureId)
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputTextureId);
        glBindSampler(0, _sampler);
        shader.setIntUniform("tex", 0);

        quad.draw();

        //Отсоединяем сэмплер и шейдерную программу
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawScene(const ShaderProgram& shader, const CameraInfo& camera)
    {
        glFrontFace(GL_CW);

        shader.setMat4Uniform("modelMatrix", cube.modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * cube.modelMatrix()))));

        cube.draw();

        shader.setMat4Uniform("modelMatrix", sphere.modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * sphere.modelMatrix()))));

        sphere.draw();
                
        shader.setMat4Uniform("modelMatrix", ground.modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * ground.modelMatrix()))));

        ground.draw();

        glFrontFace(GL_CCW);

        shader.setMat4Uniform("modelMatrix", bunny.modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * bunny.modelMatrix()))));
        
        bunny.draw();
    }

    void drawDebug()
    {        
        glClear(GL_DEPTH_BUFFER_BIT);

        int size = 500;

        if (_showGBufferDebug)
        {
            drawQuad(_quadDepthShader, _depthTexId, 0, 0, size, size);
            drawQuad(_quadColorShader, _normalsTexId, size, 0, size, size);
            drawQuad(_quadColorShader, _diffuseTexId, size * 2, 0, size, size);
        }
        else if (_showShadowDebug)
        {
            drawQuad(_quadDepthShader, _shadowTexId, 0, 0, size, size);
        }
        else if (_showDeferredDebug)
        {
            drawQuad(_quadColorShader, _deferredTexId, 0, 0, size, size);
        }
        else if (_showHDRDebug)
        {
            drawQuad(_quadColorShader, _brightTexId, 0, 0, size, size);
            drawQuad(_quadColorShader, _horizBlurTexId, size, 0, size, size);
            drawQuad(_quadColorShader, _vertBlurTexId, size * 2, 0, size, size);
        }
        else if (_showSSAODebug)
        {
            drawQuad(_quadColorShader, _ssaoTexId, 0, 0, size, size);
        }

        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawQuad(const ShaderProgram& shader, GLuint& texId, GLint x, GLint y, GLint width, GLint height)
    {
        glViewport(x, y, width, height);

        shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texId);
        glBindSampler(0, _sampler);
        shader.setIntUniform("tex", 0);

        quad.draw();
    }
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}