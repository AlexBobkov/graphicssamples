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
- идентификатор Vertex Array Object (хранит настройки буферов и вершинных атрибутов)
- количество вершин в модели
- матрица модели (LocalToWorld)
*/
class Mesh
{
public:
	Mesh();

	GLuint& primitiveType() { return _primitiveType; }

	/**
	Возвращает матрицу модели, которая описывает положение меша в мировой системе координат
	*/
	glm::mat4& modelMatrix() { return _modelMatrix; }

	/**
	Запускает отрисовку меша
	*/
	void draw();

	//----------------- Методы для инициализации меша

	/**
	Инициализирует меш сферы
	*/
	void makeSphere(float radius, int N = 100);	

	/**
	Инициализирует меш куба
	*/
	void makeCube(float size);

	/**
	Создает квадрат из двух треугольников. Координаты в Clip Space
	*/
	void makeScreenAlignedQuad();

	/**
	Создает плоскость земли размером от -size до +size по осям XY
	Генерирует текстурные координаты, так чтобы на плоскости размещалось 2 * numTiles по каждой оси
	*/
	void makeGroundPlane(float size, float numTiles);

	/**
	Загружает меш из внешнего файла с помощью библиотеки Assimp
	*/
	void loadFromFile(const std::string& filename);	

protected:
	GLuint _primitiveType;
	GLuint _vao;
	unsigned int _numVertices;
	glm::mat4 _modelMatrix;
};