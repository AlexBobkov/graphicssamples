#pragma once

#include <GL/glew.h>

#include <string>
#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
Описание меша:
- тип примитива (обычно GL_TRIANGLES)
- идентификатор Vertex Array Object (хранит настройки буферов)
- количество вершин в модели
- матрица модели (LocalToWorld)
*/
class Mesh
{
public:
	Mesh();

	/**
	Возвращает матрицу модели, которая описывает положение меша в мировой системе координат
	*/
	glm::mat4& modelMatrix() { return _modelMatrix; }

	/**
	Инициализирует меш сферы
	*/
	void makeSphere(float radius);	

	/**
	Инициализирует меш куба
	*/
	void makeCube(float size);	

	/**
	Загружает меш из внешнего файла с помощью библиотеки Assimp
	*/
	void loadFromFile(const std::string& filename);

	/**
	Запускает отрисовку меша
	*/
	void draw();

protected:
	GLuint _primitiveType;
	GLuint _vao;
	int _numVertices;

	glm::mat4 _modelMatrix;
};

typedef std::shared_ptr<Mesh> MeshPtr;