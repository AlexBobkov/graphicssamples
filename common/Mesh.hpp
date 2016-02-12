#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

#include <string>
#include <vector>
#include <set>
#include <memory>

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
    virtual ~Mesh();

    GLuint& primitiveType() { return _primitiveType; }

    /**
    Возвращает матрицу модели, которая описывает положение меша в мировой системе координат
    */
    glm::mat4& modelMatrix() { return _modelMatrix; }

    /**
    Запускает отрисовку меша
    */
    void draw();

    void drawInstanced(unsigned int N);

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

    /**
    Для примера с инстансингом. Загружает меш из внешнего файла и размножает его _positions.size() раз.
    */
    void loadFromFileArray(const std::string& filename, const std::vector<glm::vec3>& positions);

    /**
    Для примера с инстансингом. Добавляет ещё один вершинный атрибут с разделителем (divisor)
    */
    void addInstancedData(int attrNum, const std::vector<glm::vec3>& positions);

protected:
    Mesh(GLuint primitiveType, unsigned int vertexCount);

    Mesh(const Mesh&) = delete;
    void operator=(const Mesh&) = delete;

    GLuint _vao;
    GLuint _primitiveType;
    unsigned int _vertexCount;

    glm::mat4 _modelMatrix;
};

/**
Абстракция буфера с данными
*/
class VertexBuffer
{
public:
    VertexBuffer();
    ~VertexBuffer();

    void setData(GLsizeiptr size, const GLvoid* data);

    void bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    }

    void unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

protected:
    VertexBuffer(const VertexBuffer&) = delete;
    void operator=(const VertexBuffer&) = delete;

    GLuint _vbo;
};

typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;

/**
Абстракция полигональной модели
Инкапсулирует:
- управляющий объект VertexArrayObject
- буферы с вершинными атрибутами VertexBufferObject (один буфер может содержать данные для нескольких атрибутов)
- тип рисуемых примитивов
- количество вершин в модели
- матрицу модели
*/
class StrongMesh
{
public:
    StrongMesh();
    ~StrongMesh();

    void setAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset, const VertexBufferPtr& buffer);

    void setPrimitiveType(GLuint type) { _primitiveType = type; }
    void setVertexCount(GLuint count) { _vertexCount = count; }

    glm::mat4 modelMatrix() const { return _modelMatrix; }
    void setModelMatrix(const glm::mat4& m) { _modelMatrix = m; }

    void draw();

protected:
    StrongMesh(const StrongMesh&) = delete;
    void operator=(const StrongMesh&) = delete;

    GLuint _vao;

    std::set<VertexBufferPtr> _buffers;

    GLuint _primitiveType;
    GLuint _vertexCount;

    glm::mat4 _modelMatrix;
};

typedef std::shared_ptr<StrongMesh> MeshPtr;

//=========== Функции для создания тестовых мешей

MeshPtr makeSphere(float radius, unsigned int N);

MeshPtr makeCube(float size);

MeshPtr makeScreenAlignedQuad();

MeshPtr makeGroundPlane(float size, float numTiles);

MeshPtr loadFromFile(const std::string& filename);