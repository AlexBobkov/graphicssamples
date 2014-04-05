#pragma once

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AntTweakBar.h>

#include "ScreenAlignedQuadMaterial.h"
#include "RenderToGBufferMaterial.h"
#include "RenderToShadowMaterial.h"
#include "ColorMaterial.h"
#include "DeferredRenderingMaterial.h"
#include "ToneMappingEffect.h"
#include "BloomEffect.h"
#include "SSAOEffect.h"
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"

class Application
{
public:
	Application();
	~Application();

	//Инициализация графического контекста
	void initContext();

	//Настройка некоторых параметров OpenGL
	void initGL();

	void initOthers();

	//Создание трехмерной сцены
	void makeScene();
		
	//Цикл рендеринга
	void run();

	//Отрисовать один кадр
	void draw();

	//Обновление
	void update();	

	Camera& getMainCamera() { return _mainCamera; }

	void setWindowSize(int width, int height);

protected:
	GLFWwindow* _window;
	
	//Классы для загрузи шейдеров для разных материалов и эффектов
	ColorMaterial _colorMaterial;	
	RenderToShadowMaterial _renderToShadowMaterial;	
	RenderToGBufferMaterial _renderToGBufferMaterial;	
	DeferredRenderingMaterial _deferredRenderingMaterial;
	ScreenAlignedQuadMaterial _screenAlignedMaterial;
	ScreenAlignedQuadMaterial _grayscaleEffect;
	ScreenAlignedQuadMaterial _gammaEffect;
	ToneMappingEffect _toneMappingEffect;
	BloomEffect _brightPass;
	BloomEffect _horizBlurPass;
	BloomEffect _vertBlurPass;
	SSAOEffect _ssaoEffect;

	Camera _mainCamera;
	Camera _lightCamera;

	//параметры освещения
	float _lightTheta;
	float _lightPhi;
	float _lightR;
	float _ambientIntensity;
	float _diffuseIntensity;
	float _specularIntensity;
	Light _light;

	//второй источник света
	Light _light2;

	//идентификаторы текстурных объектов
	GLuint _worldTexId;
	GLuint _brickTexId;
	GLuint _grassTexId;
	GLuint _colorTexId;
	GLuint _rotateTexId;
		
	//текстуры для карты теней
	GLuint _shadowMapTexId;

	//======= Текстуры, которые должны растягиваться на весь экран VVV
	//текстуры для G-буфера
	GLuint _depthTexId;
	GLuint _normalsTexId;
	GLuint _diffuseTexId;

	//прочие текстуры для рендеринга в текстуру
	GLuint _originImageTexId;
	GLuint _toneMappedImageTexId;
	GLuint _brightImageTexId;
	GLuint _horizBlurImageTexId;
	GLuint _vertBlurImageTexId;
	GLuint _ssaoImageTexId;
	GLuint _dofHorizBlurImageTexId;
	GLuint _dofVertBlurImageTexId;
	//================================================================

	//параметры чтения из текстуры
	GLuint _sampler; //линейная фильтрация
	GLuint _pixelPreciseSampler; //фильтрация NEAREST
	GLuint _depthSampler;
	GLuint _repeatSampler;

	//полигональные 3д-модели
	Mesh _sphere;
	Mesh _cube;
	Mesh _backgroundCube;
	Mesh _plane;
	Mesh _ground;
	Mesh _bunny;
	Mesh _teapot;
	Mesh _screenQuad;
	Mesh _sphereMarker;

	std::vector<glm::vec3> _positions;

	//Идентификаторы фреймбуферов
	GLuint _shadowFramebufferId;
	GLuint _GBufferFramebufferId;
	GLuint _originImageFramebufferId;
	GLuint _toneMappingFramebufferId;
	GLuint _brightFramebufferId;
	GLuint _horizBlurFramebufferId;
	GLuint _vertBlurFramebufferId;
	GLuint _ssaoFramebufferId;	
	GLuint _dofHorizBlurFramebufferId;
	GLuint _dofVertBlurFramebufferId;

	float _exposure;

	float _focalDistance;
	float _focalRange;

	int _shadowMapWidth;
	int _shadowMapHeight;

	float _oldTime;

	int _width;
	int _height;

	TwBar* _bar;
					
	void makeSceneImplementation();

	//Инициализация фреймбуферов
	void initShadowFramebuffer();
	void initGBufferFramebuffer();
	void initOriginImageFramebuffer();
	void initToneMappingFramebuffer();
	void initBloomFramebuffer();
	void initSSAOFramebuffer();
	void initDOFFramebuffer();

	//Рендеринг в теневую карту
	void renderToShadowMap(Camera& lightCamera, GLuint fbId);

	//Рендеринг в G-буфер
	void renderToGBuffer(Camera& mainCamera, GLuint fbId);	

	//Рендеринг в текстуру эффекта SSAO (интенсивность ambient-освещения)
	void renderSSAO(Camera& mainCamera, GLuint fbId);

	//Рендеринг отложенного освещения (может использовать SSAO)
	void renderDeferred(Camera& mainCamera, Camera& lightCamera, GLuint fbId);
	
	//Рендеринг текстур для Bloom-эффекта (обрезание яркости, размытие по горизонтали и потом по вертикали)
	void renderBloom();

	//Рендеринг размытой текстуры для эффекта Depth of Field (по умолчанию в коде закомментарено)
	void renderDofBlur();

	//Рендеринг в текстуру эффекта Tone Mapping: отображение HDR в LDR. При этом используется Bloom-текстура. Также шейдер включает код для эффекта Depth of Field, но по умолчанию он закомментарен
	void renderToneMapping(Camera& mainCamera, GLuint fbId);

	//Финальный рендеринг на экран. Могут применяться эффекты grayscale или gamma correction
	void renderFinal(GLuint fbId, GLuint texId);

	//Рендеринг текстуры в левый нижний угол для отладки
	void renderDebug(int x, int y, int width, int height, GLuint texId);
};