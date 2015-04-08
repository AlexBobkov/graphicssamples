#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <vector>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

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
_primitiveType(GL_TRIANGLES),
_vao(0),
_numVertices(0)
{
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

			_numVertices += 3;

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

			_numVertices += 3;
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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(_numVertices * 3 * 4)); //сдвиг = число вершин * число компонентов (x, y, z) * размер одного компонента (float 4 байта)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(_numVertices * 3 * 4 * 2));

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

	_numVertices = 36;

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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(_numVertices * 3 * 4)); //сдвиг = число вершин * число компонентов (x, y, z) * размер одного компонента (float 4 байта)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(_numVertices * 3 * 4 * 2));

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

	_numVertices = 6;

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

	_numVertices = 6;

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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(_numVertices * 3 * 4));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(_numVertices * 3 * 4 * 2));

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

	_numVertices = mesh->mNumVertices;

	std::vector<float> vertices(_numVertices * 3);
	std::vector<float> normals(_numVertices * 3);
	std::vector<float> texcoords(_numVertices * 2);

	for (unsigned int i = 0; i < _numVertices; i++)
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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(_numVertices * 3 * 4)); //сдвиг = число вершин * число компонентов (x, y, z) * размер одного компонента (float 4 байта)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(_numVertices * 3 * 4 * 2));

	glBindVertexArray(0);
}

void Mesh::draw()
{
	glBindVertexArray(_vao);
	glDrawArrays(_primitiveType, 0, _numVertices);
}
