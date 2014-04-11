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
#include "ColorMaterial.h"
#include "CommonMaterial.h"
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include "ParticleMaterial.h"
#include "TransformFeedbackShader.h"

class Particle
{
public:
	glm::vec3 position;
	glm::vec3 velocity;
	float startTime;
};

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
	CommonMaterial _commonMaterial;
	ScreenAlignedQuadMaterial _screenAlignedMaterial;
	ParticleMaterial _particleMaterial;
	ParticleMaterial _particleTFMaterial;
	TransformFeedbackShader _tfShader;

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

	//идентификаторы текстурных объектов
	GLuint _worldTexId;
	GLuint _brickTexId;
	GLuint _grassTexId;
	GLuint _colorTexId;
	GLuint _rotateTexId;
	GLuint _texBufferId;
	GLuint _particleTexId;

	//================================================================

	//параметры чтения из текстуры
	GLuint _sampler; //линейная фильтрация
	GLuint _pixelPreciseSampler; //фильтрация NEAREST
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
	Mesh _sphereArray;

	GLuint _particlePosVbo;
	GLuint _particleTimeVbo;
	GLuint _particleVao;
	std::vector<float> _particlePositions;
	std::vector<float> _particleVelocities;
	std::vector<float> _particleTimes;
	std::vector<Particle> _particles;

	GLuint _particleVaoTF[2];
	GLuint _particlePosVboTF[2];
	GLuint _particleVelVboTF[2];
	GLuint _particleTimeVboTF[2];
	GLuint _TF[2];

	int _tfIndex;
	bool _firstTime;

	std::vector<glm::vec3> _positions;

	float _oldTime;
	float _fps;
	float _deltaTime;

	int _width;
	int _height;

	TwBar* _bar;
					
	void makeSceneImplementation();

	void drawScene(Camera& camera);
	void drawParticles(Camera& camera);
	void drawParticlesWithTransformFeedback(Camera& camera);
};