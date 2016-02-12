#define _USE_MATH_DEFINES

#include <Mesh.hpp>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <vector>

/**
Вспомогательный класс для добавления вершинных атрибутов в буфер
*/
template <typename T>
class Buffer : public std::vector < T >
{
public:
    void addVec2(T s, T t)
    {
        this->push_back(s);
        this->push_back(t);
    }

    void addVec3(T x, T y, T z)
    {
        this->push_back(x);
        this->push_back(y);
        this->push_back(z);
    }

    void addVec4(T r, T g, T b, T a)
    {
        this->push_back(r);
        this->push_back(g);
        this->push_back(b);
        this->push_back(a);
    }
};

//=========================================================

Mesh::Mesh() :
_vao(0),
_primitiveType(GL_TRIANGLES),
_vertexCount(0)
{
}

Mesh::Mesh(GLuint primitiveType, unsigned int vertexCount):
_vao(0),
_primitiveType(primitiveType),
_vertexCount(vertexCount)
{
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &_vao);
}

void Mesh::makeSphere(float radius, int N)
{
    int M = N / 2;

    Buffer<float> vertices;
    Buffer<float> normals;
    Buffer<float> texcoords;
    for (int i = 0; i < M; i++)
    {
        float theta = (float)M_PI * i / M;
        float theta1 = (float)M_PI * (i + 1) / M;

        for (int j = 0; j < N; j++)
        {
            float phi = 2.0f * (float)M_PI * j / N + (float)M_PI;
            float phi1 = 2.0f * (float)M_PI * (j + 1) / N + (float)M_PI;

            //Первый треугольник, образующий квад
            vertices.addVec3(cos(phi) * sin(theta) * radius, sin(phi) * sin(theta) * radius, cos(theta) * radius);
            vertices.addVec3(cos(phi1) * sin(theta) * radius, sin(phi1) * sin(theta) * radius, cos(theta) * radius);
            vertices.addVec3(cos(phi1) * sin(theta1) * radius, sin(phi1) * sin(theta1) * radius, cos(theta1) * radius);

            normals.addVec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
            normals.addVec3(cos(phi1) * sin(theta), sin(phi1) * sin(theta), cos(theta));
            normals.addVec3(cos(phi1) * sin(theta1), sin(phi1) * sin(theta1), cos(theta1));

            texcoords.addVec2((float)j / N, 1.0f - (float)i / M);
            texcoords.addVec2((float)(j + 1) / N, 1.0f - (float)i / M);
            texcoords.addVec2((float)(j + 1) / N, 1.0f - (float)(i + 1) / M);

            _vertexCount += 3;

            //Второй треугольник, образующий квад
            vertices.addVec3(cos(phi) * sin(theta) * radius, sin(phi) * sin(theta) * radius, cos(theta) * radius);
            vertices.addVec3(cos(phi1) * sin(theta1) * radius, sin(phi1) * sin(theta1) * radius, cos(theta1) * radius);
            vertices.addVec3(cos(phi) * sin(theta1) * radius, sin(phi) * sin(theta1) * radius, cos(theta1) * radius);

            normals.addVec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
            normals.addVec3(cos(phi1) * sin(theta1), sin(phi1) * sin(theta1), cos(theta1));
            normals.addVec3(cos(phi) * sin(theta1), sin(phi) * sin(theta1), cos(theta1));

            texcoords.addVec2((float)j / N, 1.0f - (float)i / M);
            texcoords.addVec2((float)(j + 1) / N, 1.0f - (float)(i + 1) / M);
            texcoords.addVec2((float)j / N, 1.0f - (float)(i + 1) / M);

            _vertexCount += 3;
        }
    }

    vertices.insert(vertices.end(), normals.begin(), normals.end());
    vertices.insert(vertices.end(), texcoords.begin(), texcoords.end());

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    _vao = 0;
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(_vertexCount * 3 * 4)); //сдвиг = число вершин * число компонентов (x, y, z) * размер одного компонента (float 4 байта)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(_vertexCount * 3 * 4 * 2));

    glBindVertexArray(0);
}

void Mesh::makeCube(float size)
{
    Buffer<float> vertices;
    Buffer<float> normals;
    Buffer<float> texcoords;

    //front 1
    vertices.addVec3(size, -size, size);
    vertices.addVec3(size, size, size);
    vertices.addVec3(size, size, -size);

    normals.addVec3(1.0, 0.0, 0.0);
    normals.addVec3(1.0, 0.0, 0.0);
    normals.addVec3(1.0, 0.0, 0.0);

    texcoords.addVec2(0.0, 1.0);
    texcoords.addVec2(1.0, 1.0);
    texcoords.addVec2(1.0, 0.0);

    //front 2
    vertices.addVec3(size, -size, size);
    vertices.addVec3(size, size, -size);
    vertices.addVec3(size, -size, -size);

    normals.addVec3(1.0, 0.0, 0.0);
    normals.addVec3(1.0, 0.0, 0.0);
    normals.addVec3(1.0, 0.0, 0.0);

    texcoords.addVec2(0.0, 1.0);
    texcoords.addVec2(1.0, 0.0);
    texcoords.addVec2(0.0, 0.0);

    //left 1
    vertices.addVec3(-size, -size, size);
    vertices.addVec3(size, -size, size);
    vertices.addVec3(size, -size, -size);

    normals.addVec3(0.0, -1.0, 0.0);
    normals.addVec3(0.0, -1.0, 0.0);
    normals.addVec3(0.0, -1.0, 0.0);

    texcoords.addVec2(0.0, 1.0);
    texcoords.addVec2(1.0, 1.0);
    texcoords.addVec2(1.0, 0.0);

    //left 2
    vertices.addVec3(-size, -size, size);
    vertices.addVec3(size, -size, -size);
    vertices.addVec3(-size, -size, -size);

    normals.addVec3(0.0, -1.0, 0.0);
    normals.addVec3(0.0, -1.0, 0.0);
    normals.addVec3(0.0, -1.0, 0.0);

    texcoords.addVec2(0.0, 1.0);
    texcoords.addVec2(1.0, 0.0);
    texcoords.addVec2(0.0, 0.0);

    //top 1
    vertices.addVec3(-size, size, size);
    vertices.addVec3(size, size, size);
    vertices.addVec3(size, -size, size);

    normals.addVec3(0.0, 0.0, 1.0);
    normals.addVec3(0.0, 0.0, 1.0);
    normals.addVec3(0.0, 0.0, 1.0);

    texcoords.addVec2(0.0, 1.0);
    texcoords.addVec2(1.0, 1.0);
    texcoords.addVec2(1.0, 0.0);

    //top 2
    vertices.addVec3(-size, size, size);
    vertices.addVec3(size, -size, size);
    vertices.addVec3(-size, -size, size);

    normals.addVec3(0.0, 0.0, 1.0);
    normals.addVec3(0.0, 0.0, 1.0);
    normals.addVec3(0.0, 0.0, 1.0);

    texcoords.addVec2(0.0, 1.0);
    texcoords.addVec2(1.0, 0.0);
    texcoords.addVec2(0.0, 0.0);

    //back 1
    vertices.addVec3(-size, -size, size);
    vertices.addVec3(-size, size, -size);
    vertices.addVec3(-size, size, size);

    normals.addVec3(-1.0, 0.0, 0.0);
    normals.addVec3(-1.0, 0.0, 0.0);
    normals.addVec3(-1.0, 0.0, 0.0);

    texcoords.addVec2(0.0, 1.0);
    texcoords.addVec2(1.0, 0.0);
    texcoords.addVec2(1.0, 1.0);

    //back 2
    vertices.addVec3(-size, -size, size);
    vertices.addVec3(-size, -size, -size);
    vertices.addVec3(-size, size, -size);

    normals.addVec3(-1.0, 0.0, 0.0);
    normals.addVec3(-1.0, 0.0, 0.0);
    normals.addVec3(-1.0, 0.0, 0.0);

    texcoords.addVec2(0.0, 1.0);
    texcoords.addVec2(0.0, 0.0);
    texcoords.addVec2(1.0, 0.0);

    //right 1
    vertices.addVec3(-size, size, size);
    vertices.addVec3(size, size, -size);
    vertices.addVec3(size, size, size);

    normals.addVec3(0.0, 1.0, 0.0);
    normals.addVec3(0.0, 1.0, 0.0);
    normals.addVec3(0.0, 1.0, 0.0);

    texcoords.addVec2(0.0, 1.0);
    texcoords.addVec2(1.0, 0.0);
    texcoords.addVec2(1.0, 1.0);

    //right 2
    vertices.addVec3(-size, size, size);
    vertices.addVec3(-size, size, -size);
    vertices.addVec3(+size, size, -size);

    normals.addVec3(0.0, 1.0, 0.0);
    normals.addVec3(0.0, 1.0, 0.0);
    normals.addVec3(0.0, 1.0, 0.0);

    texcoords.addVec2(0.0, 1.0);
    texcoords.addVec2(0.0, 0.0);
    texcoords.addVec2(1.0, 0.0);

    //bottom 1
    vertices.addVec3(-size, size, -size);
    vertices.addVec3(size, -size, -size);
    vertices.addVec3(size, size, -size);

    normals.addVec3(0.0, 0.0, -1.0);
    normals.addVec3(0.0, 0.0, -1.0);
    normals.addVec3(0.0, 0.0, -1.0);

    texcoords.addVec2(0.0, 1.0);
    texcoords.addVec2(1.0, 0.0);
    texcoords.addVec2(1.0, 1.0);

    //bottom 2
    vertices.addVec3(-size, size, -size);
    vertices.addVec3(-size, -size, -size);
    vertices.addVec3(size, -size, -size);

    normals.addVec3(0.0, 0.0, -1.0);
    normals.addVec3(0.0, 0.0, -1.0);
    normals.addVec3(0.0, 0.0, -1.0);

    texcoords.addVec2(0.0, 1.0);
    texcoords.addVec2(0.0, 0.0);
    texcoords.addVec2(1.0, 0.0);

    vertices.insert(vertices.end(), normals.begin(), normals.end());
    vertices.insert(vertices.end(), texcoords.begin(), texcoords.end());

    _vertexCount = 36;

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    _vao = 0;
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(_vertexCount * 3 * 4)); //сдвиг = число вершин * число компонентов (x, y, z) * размер одного компонента (float 4 байта)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(_vertexCount * 3 * 4 * 2));

    glBindVertexArray(0);
}

void Mesh::makeScreenAlignedQuad()
{
    Buffer<float> vertices;

    //front 1
    vertices.addVec3(-1.0, 1.0, 0.0);
    vertices.addVec3(1.0, 1.0, 0.0);
    vertices.addVec3(1.0, -1.0, 0.0);

    //front 2
    vertices.addVec3(-1.0, 1.0, 0.0);
    vertices.addVec3(1.0, -1.0, 0.0);
    vertices.addVec3(-1.0, -1.0, 0.0);

    _vertexCount = 6;

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    _vao = 0;
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindVertexArray(0);
}

void Mesh::makeGroundPlane(float size, float numTiles)
{
    Buffer<float> vertices;
    Buffer<float> normals;
    Buffer<float> texcoords;

    //front 1
    vertices.addVec3(-size, size, 0.0);
    vertices.addVec3(size, size, 0.0);
    vertices.addVec3(size, -size, 0.0);

    normals.addVec3(0.0, 0.0, 1.0);
    normals.addVec3(0.0, 0.0, 1.0);
    normals.addVec3(0.0, 0.0, 1.0);

    texcoords.addVec2(-numTiles, numTiles);
    texcoords.addVec2(numTiles, numTiles);
    texcoords.addVec2(numTiles, -numTiles);

    //front 2
    vertices.addVec3(-size, size, 0.0);
    vertices.addVec3(size, -size, 0.0);
    vertices.addVec3(-size, -size, 0.0);

    normals.addVec3(0.0, 0.0, 1.0);
    normals.addVec3(0.0, 0.0, 1.0);
    normals.addVec3(0.0, 0.0, 1.0);

    texcoords.addVec2(-numTiles, numTiles);
    texcoords.addVec2(numTiles, -numTiles);
    texcoords.addVec2(-numTiles, -numTiles);

    vertices.insert(vertices.end(), normals.begin(), normals.end());
    vertices.insert(vertices.end(), texcoords.begin(), texcoords.end());

    _vertexCount = 6;

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    _vao = 0;
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(_vertexCount * 3 * 4));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(_vertexCount * 3 * 4 * 2));

    glBindVertexArray(0);
}


void Mesh::loadFromFile(const std::string& filename)
{
    const struct aiScene* scene = aiImportFile(filename.c_str(), aiProcess_Triangulate);

    if (!scene)
    {
        std::cerr << aiGetErrorString() << std::endl;
        return;
    }

    if (scene->mNumMeshes == 0)
    {
        std::cerr << "There is no meshes in file " << filename << std::endl;;
        return;
    }

    const struct aiMesh* mesh = scene->mMeshes[0];

    if (!mesh->HasPositions())
    {
        std::cerr << "This demo does not support meshes without positions\n";
        return;
    }

    if (!mesh->HasNormals())
    {
        std::cerr << "This demo does not support meshes without normals\n";
        return;
    }

    if (!mesh->HasTextureCoords(0))
    {
        std::cerr << "This demo does not support meshes without texcoords for texture unit 0\n";
        return;
    }

    _vertexCount = mesh->mNumVertices;

    std::vector<float> vertices(_vertexCount * 3);
    std::vector<float> normals(_vertexCount * 3);
    std::vector<float> texcoords(_vertexCount * 2);

    for (unsigned int i = 0; i < _vertexCount; i++)
    {
        const aiVector3D* vp = &(mesh->mVertices[i]);

        vertices[i * 3 + 0] = (GLfloat)vp->x;
        vertices[i * 3 + 1] = (GLfloat)vp->y;
        vertices[i * 3 + 2] = (GLfloat)vp->z;

        const aiVector3D* normal = &(mesh->mNormals[i]);

        normals[i * 3 + 0] = (GLfloat)normal->x;
        normals[i * 3 + 1] = (GLfloat)normal->y;
        normals[i * 3 + 2] = (GLfloat)normal->z;

        const aiVector3D* tc = &(mesh->mTextureCoords[0][i]);

        texcoords[i * 2 + 0] = (GLfloat)tc->x;
        texcoords[i * 2 + 1] = (GLfloat)tc->y;
    }

    aiReleaseImport(scene);

    vertices.insert(vertices.end(), normals.begin(), normals.end());
    vertices.insert(vertices.end(), texcoords.begin(), texcoords.end());

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    _vao = 0;
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(_vertexCount * 3 * 4)); //сдвиг = число вершин * число компонентов (x, y, z) * размер одного компонента (float 4 байта)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(_vertexCount * 3 * 4 * 2));

    glBindVertexArray(0);
}

void Mesh::loadFromFileArray(const std::string& filename, const std::vector<glm::vec3>& positions)
{
    const struct aiScene* scene = aiImportFile(filename.c_str(), aiProcess_Triangulate);

    if (!scene)
    {
        std::cerr << aiGetErrorString() << std::endl;
        return;
    }

    if (scene->mNumMeshes == 0)
    {
        std::cerr << "There is no meshes in file " << filename << std::endl;;
        return;
    }

    const struct aiMesh* mesh = scene->mMeshes[0];

    if (!mesh->HasPositions())
    {
        std::cerr << "This demo does not support meshes without positions\n";
        return;
    }

    if (!mesh->HasNormals())
    {
        std::cerr << "This demo does not support meshes without normals\n";
        return;
    }

    if (!mesh->HasTextureCoords(0))
    {
        std::cerr << "This demo does not support meshes without texcoords for texture unit 0\n";
        return;
    }

    _vertexCount = mesh->mNumVertices * positions.size();

    std::vector<float> vertices(_vertexCount * 3);
    std::vector<float> normals(_vertexCount * 3);
    std::vector<float> texcoords(_vertexCount * 2);

    for (unsigned int k = 0; k < positions.size(); k++)
    {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            const aiVector3D* vp = &(mesh->mVertices[i]);

            vertices[i * 3 + 0 + k * mesh->mNumVertices * 3] = (GLfloat)vp->x + positions[k].x;
            vertices[i * 3 + 1 + k * mesh->mNumVertices * 3] = (GLfloat)vp->y + positions[k].y;
            vertices[i * 3 + 2 + k * mesh->mNumVertices * 3] = (GLfloat)vp->z + positions[k].z;

            const aiVector3D* normal = &(mesh->mNormals[i]);

            normals[i * 3 + 0 + k * mesh->mNumVertices * 3] = (GLfloat)normal->x;
            normals[i * 3 + 1 + k * mesh->mNumVertices * 3] = (GLfloat)normal->y;
            normals[i * 3 + 2 + k * mesh->mNumVertices * 3] = (GLfloat)normal->z;

            const aiVector3D* tc = &(mesh->mTextureCoords[0][i]);

            texcoords[i * 2 + 0 + k * mesh->mNumVertices * 2] = (GLfloat)tc->x;
            texcoords[i * 2 + 1 + k * mesh->mNumVertices * 2] = (GLfloat)tc->y;
        }
    }

    aiReleaseImport(scene);

    vertices.insert(vertices.end(), normals.begin(), normals.end());
    vertices.insert(vertices.end(), texcoords.begin(), texcoords.end());

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    _vao = 0;
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(_vertexCount * 3 * 4)); //сдвиг = число вершин * число компонентов (x, y, z) * размер одного компонента (float 4 байта)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(_vertexCount * 3 * 4 * 2));

    glBindVertexArray(0);
}

void Mesh::addInstancedData(int attrNum, const std::vector<glm::vec3>& positions)
{
    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); i++)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
    }

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    glBindVertexArray(_vao);
    glEnableVertexAttribArray(attrNum);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(attrNum, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribDivisor(attrNum, 1);

    glBindVertexArray(0);
}

void Mesh::draw()
{
    glBindVertexArray(_vao);
    glDrawArrays(_primitiveType, 0, _vertexCount);
}

void Mesh::drawInstanced(unsigned int N)
{
    glBindVertexArray(_vao);
    glDrawArraysInstanced(_primitiveType, 0, _vertexCount, N);
}

//==========================================

VertexBuffer::VertexBuffer()
{
    glGenBuffers(1, &_vbo);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &_vbo);
}

void VertexBuffer::setData(GLsizeiptr size, const GLvoid* data)
{
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//==================================

StrongMesh::StrongMesh():
_primitiveType(GL_TRIANGLES),
_vertexCount(0)
{
    glGenVertexArrays(1, &_vao);
}

StrongMesh::~StrongMesh()
{
    glDeleteVertexArrays(1, &_vao);
}

void StrongMesh::setAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint offset, const VertexBufferPtr& buffer)
{
    _buffers.insert(buffer);

    glBindVertexArray(_vao);
    
    buffer->bind();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<void*>(offset));
    buffer->unbind();

    glBindVertexArray(0);
}

void StrongMesh::draw()
{
    glBindVertexArray(_vao);
    glDrawArrays(_primitiveType, 0, _vertexCount);
}

MeshPtr makeSphere(float radius, unsigned int N)
{
    unsigned int M = N / 2;
       
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    
    for (unsigned int i = 0; i < M; i++)
    {
        float theta = (float)M_PI * i / M;
        float theta1 = (float)M_PI * (i + 1) / M;

        for (unsigned int j = 0; j < N; j++)
        {
            float phi = 2.0f * (float)M_PI * j / N + (float)M_PI;
            float phi1 = 2.0f * (float)M_PI * (j + 1) / N + (float)M_PI;

            //Первый треугольник, образующий квад
            vertices.push_back(glm::vec3(cos(phi) * sin(theta) * radius, sin(phi) * sin(theta) * radius, cos(theta) * radius));
            vertices.push_back(glm::vec3(cos(phi1) * sin(theta) * radius, sin(phi1) * sin(theta) * radius, cos(theta) * radius));
            vertices.push_back(glm::vec3(cos(phi1) * sin(theta1) * radius, sin(phi1) * sin(theta1) * radius, cos(theta1) * radius));

            normals.push_back(glm::vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta)));
            normals.push_back(glm::vec3(cos(phi1) * sin(theta), sin(phi1) * sin(theta), cos(theta)));
            normals.push_back(glm::vec3(cos(phi1) * sin(theta1), sin(phi1) * sin(theta1), cos(theta1)));

            texcoords.push_back(glm::vec2((float)j / N, 1.0f - (float)i / M));
            texcoords.push_back(glm::vec2((float)(j + 1) / N, 1.0f - (float)i / M));
            texcoords.push_back(glm::vec2((float)(j + 1) / N, 1.0f - (float)(i + 1) / M));

            //Второй треугольник, образующий квад
            vertices.push_back(glm::vec3(cos(phi) * sin(theta) * radius, sin(phi) * sin(theta) * radius, cos(theta) * radius));
            vertices.push_back(glm::vec3(cos(phi1) * sin(theta1) * radius, sin(phi1) * sin(theta1) * radius, cos(theta1) * radius));
            vertices.push_back(glm::vec3(cos(phi) * sin(theta1) * radius, sin(phi) * sin(theta1) * radius, cos(theta1) * radius));

            normals.push_back(glm::vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta)));
            normals.push_back(glm::vec3(cos(phi1) * sin(theta1), sin(phi1) * sin(theta1), cos(theta1)));
            normals.push_back(glm::vec3(cos(phi) * sin(theta1), sin(phi) * sin(theta1), cos(theta1)));

            texcoords.push_back(glm::vec2((float)j / N, 1.0f - (float)i / M));
            texcoords.push_back(glm::vec2((float)(j + 1) / N, 1.0f - (float)(i + 1) / M));
            texcoords.push_back(glm::vec2((float)j / N, 1.0f - (float)(i + 1) / M));
        }
    }

    //----------------------------------------
        
    VertexBufferPtr buf0 = std::make_shared<VertexBuffer>();
    buf0->setData(vertices.size() * sizeof(float) * 3, vertices.data());

    VertexBufferPtr buf1 = std::make_shared<VertexBuffer>();
    buf1->setData(normals.size() * sizeof(float) * 3, normals.data());

    VertexBufferPtr buf2 = std::make_shared<VertexBuffer>();
    buf2->setData(texcoords.size() * sizeof(float) * 2, texcoords.data());

    MeshPtr mesh = std::make_shared<StrongMesh>();
    mesh->setAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0, buf0);
    mesh->setAttribute(1, 3, GL_FLOAT, GL_FALSE, 0, 0, buf1);
    mesh->setAttribute(2, 2, GL_FLOAT, GL_FALSE, 0, 0, buf2);
    mesh->setPrimitiveType(GL_TRIANGLES);
    mesh->setVertexCount(vertices.size());

    return mesh;
}

MeshPtr makeCube(float size)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;

    //front 1
    vertices.push_back(glm::vec3(size, -size, size));
    vertices.push_back(glm::vec3(size, size, size));
    vertices.push_back(glm::vec3(size, size, -size));

    normals.push_back(glm::vec3(1.0, 0.0, 0.0));
    normals.push_back(glm::vec3(1.0, 0.0, 0.0));
    normals.push_back(glm::vec3(1.0, 0.0, 0.0));

    texcoords.push_back(glm::vec2(0.0, 1.0));
    texcoords.push_back(glm::vec2(1.0, 1.0));
    texcoords.push_back(glm::vec2(1.0, 0.0));

    //front 2
    vertices.push_back(glm::vec3(size, -size, size));
    vertices.push_back(glm::vec3(size, size, -size));
    vertices.push_back(glm::vec3(size, -size, -size));

    normals.push_back(glm::vec3(1.0, 0.0, 0.0));
    normals.push_back(glm::vec3(1.0, 0.0, 0.0));
    normals.push_back(glm::vec3(1.0, 0.0, 0.0));

    texcoords.push_back(glm::vec2(0.0, 1.0));
    texcoords.push_back(glm::vec2(1.0, 0.0));
    texcoords.push_back(glm::vec2(0.0, 0.0));

    //left 1
    vertices.push_back(glm::vec3(-size, -size, size));
    vertices.push_back(glm::vec3(size, -size, size));
    vertices.push_back(glm::vec3(size, -size, -size));

    normals.push_back(glm::vec3(0.0, -1.0, 0.0));
    normals.push_back(glm::vec3(0.0, -1.0, 0.0));
    normals.push_back(glm::vec3(0.0, -1.0, 0.0));

    texcoords.push_back(glm::vec2(0.0, 1.0));
    texcoords.push_back(glm::vec2(1.0, 1.0));
    texcoords.push_back(glm::vec2(1.0, 0.0));

    //left 2
    vertices.push_back(glm::vec3(-size, -size, size));
    vertices.push_back(glm::vec3(size, -size, -size));
    vertices.push_back(glm::vec3(-size, -size, -size));

    normals.push_back(glm::vec3(0.0, -1.0, 0.0));
    normals.push_back(glm::vec3(0.0, -1.0, 0.0));
    normals.push_back(glm::vec3(0.0, -1.0, 0.0));

    texcoords.push_back(glm::vec2(0.0, 1.0));
    texcoords.push_back(glm::vec2(1.0, 0.0));
    texcoords.push_back(glm::vec2(0.0, 0.0));

    //top 1
    vertices.push_back(glm::vec3(-size, size, size));
    vertices.push_back(glm::vec3(size, size, size));
    vertices.push_back(glm::vec3(size, -size, size));

    normals.push_back(glm::vec3(0.0, 0.0, 1.0));
    normals.push_back(glm::vec3(0.0, 0.0, 1.0));
    normals.push_back(glm::vec3(0.0, 0.0, 1.0));

    texcoords.push_back(glm::vec2(0.0, 1.0));
    texcoords.push_back(glm::vec2(1.0, 1.0));
    texcoords.push_back(glm::vec2(1.0, 0.0));

    //top 2
    vertices.push_back(glm::vec3(-size, size, size));
    vertices.push_back(glm::vec3(size, -size, size));
    vertices.push_back(glm::vec3(-size, -size, size));

    normals.push_back(glm::vec3(0.0, 0.0, 1.0));
    normals.push_back(glm::vec3(0.0, 0.0, 1.0));
    normals.push_back(glm::vec3(0.0, 0.0, 1.0));

    texcoords.push_back(glm::vec2(0.0, 1.0));
    texcoords.push_back(glm::vec2(1.0, 0.0));
    texcoords.push_back(glm::vec2(0.0, 0.0));

    //back 1
    vertices.push_back(glm::vec3(-size, -size, size));
    vertices.push_back(glm::vec3(-size, size, -size));
    vertices.push_back(glm::vec3(-size, size, size));

    normals.push_back(glm::vec3(-1.0, 0.0, 0.0));
    normals.push_back(glm::vec3(-1.0, 0.0, 0.0));
    normals.push_back(glm::vec3(-1.0, 0.0, 0.0));

    texcoords.push_back(glm::vec2(0.0, 1.0));
    texcoords.push_back(glm::vec2(1.0, 0.0));
    texcoords.push_back(glm::vec2(1.0, 1.0));

    //back 2
    vertices.push_back(glm::vec3(-size, -size, size));
    vertices.push_back(glm::vec3(-size, -size, -size));
    vertices.push_back(glm::vec3(-size, size, -size));

    normals.push_back(glm::vec3(-1.0, 0.0, 0.0));
    normals.push_back(glm::vec3(-1.0, 0.0, 0.0));
    normals.push_back(glm::vec3(-1.0, 0.0, 0.0));

    texcoords.push_back(glm::vec2(0.0, 1.0));
    texcoords.push_back(glm::vec2(0.0, 0.0));
    texcoords.push_back(glm::vec2(1.0, 0.0));

    //right 1
    vertices.push_back(glm::vec3(-size, size, size));
    vertices.push_back(glm::vec3(size, size, -size));
    vertices.push_back(glm::vec3(size, size, size));

    normals.push_back(glm::vec3(0.0, 1.0, 0.0));
    normals.push_back(glm::vec3(0.0, 1.0, 0.0));
    normals.push_back(glm::vec3(0.0, 1.0, 0.0));

    texcoords.push_back(glm::vec2(0.0, 1.0));
    texcoords.push_back(glm::vec2(1.0, 0.0));
    texcoords.push_back(glm::vec2(1.0, 1.0));

    //right 2
    vertices.push_back(glm::vec3(-size, size, size));
    vertices.push_back(glm::vec3(-size, size, -size));
    vertices.push_back(glm::vec3(+size, size, -size));

    normals.push_back(glm::vec3(0.0, 1.0, 0.0));
    normals.push_back(glm::vec3(0.0, 1.0, 0.0));
    normals.push_back(glm::vec3(0.0, 1.0, 0.0));

    texcoords.push_back(glm::vec2(0.0, 1.0));
    texcoords.push_back(glm::vec2(0.0, 0.0));
    texcoords.push_back(glm::vec2(1.0, 0.0));

    //bottom 1
    vertices.push_back(glm::vec3(-size, size, -size));
    vertices.push_back(glm::vec3(size, -size, -size));
    vertices.push_back(glm::vec3(size, size, -size));

    normals.push_back(glm::vec3(0.0, 0.0, -1.0));
    normals.push_back(glm::vec3(0.0, 0.0, -1.0));
    normals.push_back(glm::vec3(0.0, 0.0, -1.0));

    texcoords.push_back(glm::vec2(0.0, 1.0));
    texcoords.push_back(glm::vec2(1.0, 0.0));
    texcoords.push_back(glm::vec2(1.0, 1.0));

    //bottom 2
    vertices.push_back(glm::vec3(-size, size, -size));
    vertices.push_back(glm::vec3(-size, -size, -size));
    vertices.push_back(glm::vec3(size, -size, -size));

    normals.push_back(glm::vec3(0.0, 0.0, -1.0));
    normals.push_back(glm::vec3(0.0, 0.0, -1.0));
    normals.push_back(glm::vec3(0.0, 0.0, -1.0));

    texcoords.push_back(glm::vec2(0.0, 1.0));
    texcoords.push_back(glm::vec2(0.0, 0.0));
    texcoords.push_back(glm::vec2(1.0, 0.0));

    //----------------------------------------

    VertexBufferPtr buf0 = std::make_shared<VertexBuffer>();
    buf0->setData(vertices.size() * sizeof(float) * 3, vertices.data());

    VertexBufferPtr buf1 = std::make_shared<VertexBuffer>();
    buf1->setData(normals.size() * sizeof(float) * 3, normals.data());

    VertexBufferPtr buf2 = std::make_shared<VertexBuffer>();
    buf2->setData(texcoords.size() * sizeof(float) * 2, texcoords.data());

    MeshPtr mesh = std::make_shared<StrongMesh>();
    mesh->setAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0, buf0);
    mesh->setAttribute(1, 3, GL_FLOAT, GL_FALSE, 0, 0, buf1);
    mesh->setAttribute(2, 2, GL_FLOAT, GL_FALSE, 0, 0, buf2);
    mesh->setPrimitiveType(GL_TRIANGLES);
    mesh->setVertexCount(vertices.size());

    return mesh;
}

MeshPtr makeScreenAlignedQuad()
{
    std::vector<glm::vec3> vertices;

    //front 1
    vertices.push_back(glm::vec3(-1.0, 1.0, 0.0));
    vertices.push_back(glm::vec3(1.0, 1.0, 0.0));
    vertices.push_back(glm::vec3(1.0, -1.0, 0.0));

    //front 2
    vertices.push_back(glm::vec3(-1.0, 1.0, 0.0));
    vertices.push_back(glm::vec3(1.0, -1.0, 0.0));
    vertices.push_back(glm::vec3(-1.0, -1.0, 0.0));

    //----------------------------------------

    VertexBufferPtr buf0 = std::make_shared<VertexBuffer>();
    buf0->setData(vertices.size() * sizeof(float) * 3, vertices.data());

    MeshPtr mesh = std::make_shared<StrongMesh>();
    mesh->setAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0, buf0);
    mesh->setPrimitiveType(GL_TRIANGLES);
    mesh->setVertexCount(vertices.size());

    return mesh;
}

MeshPtr makeGroundPlane(float size, float numTiles)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;

    //front 1
    vertices.push_back(glm::vec3(-size, size, 0.0));
    vertices.push_back(glm::vec3(size, size, 0.0));
    vertices.push_back(glm::vec3(size, -size, 0.0));

    normals.push_back(glm::vec3(0.0, 0.0, 1.0));
    normals.push_back(glm::vec3(0.0, 0.0, 1.0));
    normals.push_back(glm::vec3(0.0, 0.0, 1.0));

    texcoords.push_back(glm::vec2(-numTiles, numTiles));
    texcoords.push_back(glm::vec2(numTiles, numTiles));
    texcoords.push_back(glm::vec2(numTiles, -numTiles));

    //front 2
    vertices.push_back(glm::vec3(-size, size, 0.0));
    vertices.push_back(glm::vec3(size, -size, 0.0));
    vertices.push_back(glm::vec3(-size, -size, 0.0));

    normals.push_back(glm::vec3(0.0, 0.0, 1.0));
    normals.push_back(glm::vec3(0.0, 0.0, 1.0));
    normals.push_back(glm::vec3(0.0, 0.0, 1.0));

    texcoords.push_back(glm::vec2(-numTiles, numTiles));
    texcoords.push_back(glm::vec2(numTiles, -numTiles));
    texcoords.push_back(glm::vec2(-numTiles, -numTiles));

    //----------------------------------------

    VertexBufferPtr buf0 = std::make_shared<VertexBuffer>();
    buf0->setData(vertices.size() * sizeof(float) * 3, vertices.data());

    VertexBufferPtr buf1 = std::make_shared<VertexBuffer>();
    buf1->setData(normals.size() * sizeof(float) * 3, normals.data());

    VertexBufferPtr buf2 = std::make_shared<VertexBuffer>();
    buf2->setData(texcoords.size() * sizeof(float) * 2, texcoords.data());

    MeshPtr mesh = std::make_shared<StrongMesh>();
    mesh->setAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0, buf0);
    mesh->setAttribute(1, 3, GL_FLOAT, GL_FALSE, 0, 0, buf1);
    mesh->setAttribute(2, 2, GL_FLOAT, GL_FALSE, 0, 0, buf2);
    mesh->setPrimitiveType(GL_TRIANGLES);
    mesh->setVertexCount(vertices.size());
    
    return mesh;
}

MeshPtr loadFromFile(const std::string& filename)
{
    const struct aiScene* assimpScene = aiImportFile(filename.c_str(), aiProcess_Triangulate);

    if (!assimpScene)
    {
        std::cerr << aiGetErrorString() << std::endl;
        return std::make_shared<StrongMesh>();
    }

    if (assimpScene->mNumMeshes == 0)
    {
        std::cerr << "There is no meshes in file " << filename << std::endl;
        return std::make_shared<StrongMesh>();
    }

    const struct aiMesh* assimpMesh = assimpScene->mMeshes[0];

    if (!assimpMesh->HasPositions())
    {
        std::cerr << "This demo does not support meshes without positions\n";
        return std::make_shared<StrongMesh>();
    }

    if (!assimpMesh->HasNormals())
    {
        std::cerr << "This demo does not support meshes without normals\n";
        return std::make_shared<StrongMesh>();
    }

    if (!assimpMesh->HasTextureCoords(0))
    {
        std::cerr << "This demo does not support meshes without texcoords for texture unit 0\n";
        return std::make_shared<StrongMesh>();
    }

    VertexBufferPtr buf0 = std::make_shared<VertexBuffer>();
    buf0->setData(assimpMesh->mNumVertices * sizeof(float) * 3, assimpMesh->mVertices);

    VertexBufferPtr buf1 = std::make_shared<VertexBuffer>();
    buf1->setData(assimpMesh->mNumVertices * sizeof(float) * 3, assimpMesh->mNormals);

    VertexBufferPtr buf2 = std::make_shared<VertexBuffer>();
    buf2->setData(assimpMesh->mNumVertices * sizeof(float) * 2, assimpMesh->mTextureCoords[0]);

    MeshPtr mesh = std::make_shared<StrongMesh>();
    mesh->setAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0, buf0);
    mesh->setAttribute(1, 3, GL_FLOAT, GL_FALSE, 0, 0, buf1);
    mesh->setAttribute(2, 2, GL_FLOAT, GL_FALSE, 0, 0, buf2);
    mesh->setPrimitiveType(GL_TRIANGLES);
    mesh->setVertexCount(assimpMesh->mNumVertices);

    aiReleaseImport(assimpScene);

    return mesh;
}