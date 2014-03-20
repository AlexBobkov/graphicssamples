#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

// assimp include files. These three are usually needed.
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

//=========================================================

float frand(float low, float high)
{
	return low + (high - low) * (rand() % 1000) * 0.001f;
}

void addVec2(std::vector<float>& vec, float s, float t)
{
	vec.push_back(s);
	vec.push_back(t);
}

void addVec3(std::vector<float>& vec, float x, float y, float z)
{
	vec.push_back(x);
	vec.push_back(y);
	vec.push_back(z);
}

void addVec4(std::vector<float>& vec, float r, float g, float b, float a)
{
	vec.push_back(r);
	vec.push_back(g);
	vec.push_back(b);
	vec.push_back(a);
}

//вычисление цвета по линейной палитре
void getColorFromLinearPalette(float value, float& r, float& g, float& b)
{
	if (value < 0.25f)
	{
		r = 0.0f;
		g = value * 4.0f;
		b = 1.0f;
	}
	else if (value < 0.5f)
	{
		r = 0.0f;
		g = 1.0f;
		b = (0.5f - value) * 4.0f;
	}
	else if (value < 0.75f)
	{
		r = (value - 0.5f) * 4.0f;
		g = 1.0f;
		b = 0.0f;
	}
	else
	{
		r = 1.0f;
		g = (1.0f - value) * 4.0f;
		b = 0.0f;
	}
}

//=========================================================

Mesh::Mesh():
_vao(0),
	_numVertices(0)
{
}

Mesh::Mesh(GLuint vao, int numVertices):
_vao(vao),
	_numVertices(numVertices)
{
}

Mesh Mesh::makeSphere(float radius)
{
	int N = 100;
	int M = 50;
	int numVertices = 0;

	std::vector<float> vertices;	
	std::vector<float> normals;
	std::vector<float> texcoords;
	for (int i = 0; i < M; i++)
	{
		float theta = (float)M_PI * i / M;
		float theta1 = (float)M_PI * (i + 1) / M;

		for (int j = 0; j < N; j++)
		{
			float phi = 2.0f * (float)M_PI * j / N + (float)M_PI;
			float phi1 = 2.0f * (float)M_PI * (j + 1) / N + (float)M_PI;

			//Первый треугольник, образующий квад
			addVec3(vertices, cos(phi) * sin(theta) * radius, sin(phi) * sin(theta) * radius, cos(theta) * radius);
			addVec3(vertices, cos(phi1) * sin(theta) * radius, sin(phi1) * sin(theta) * radius, cos(theta) * radius);
			addVec3(vertices, cos(phi1) * sin(theta1) * radius, sin(phi1) * sin(theta1) * radius, cos(theta1) * radius);

			addVec3(normals, cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
			addVec3(normals, cos(phi1) * sin(theta), sin(phi1) * sin(theta), cos(theta));
			addVec3(normals, cos(phi1) * sin(theta1), sin(phi1) * sin(theta1), cos(theta1));

			addVec2(texcoords, (float)j / N, 1.0f - (float)i / M);
			addVec2(texcoords, (float)(j + 1) / N, 1.0f - (float)i / M);
			addVec2(texcoords, (float)(j + 1) / N, 1.0f - (float)(i + 1) / M);						

			numVertices += 3;

			//Второй треугольник, образующий квад
			addVec3(vertices, cos(phi) * sin(theta) * radius, sin(phi) * sin(theta) * radius, cos(theta) * radius);
			addVec3(vertices, cos(phi1) * sin(theta1) * radius, sin(phi1) * sin(theta1) * radius, cos(theta1) * radius);
			addVec3(vertices, cos(phi) * sin(theta1) * radius, sin(phi) * sin(theta1) * radius, cos(theta1) * radius);			

			addVec3(normals, cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
			addVec3(normals, cos(phi1) * sin(theta1), sin(phi1) * sin(theta1), cos(theta1));
			addVec3(normals, cos(phi) * sin(theta1), sin(phi) * sin(theta1), cos(theta1));			

			addVec2(texcoords, (float)j / N, 1.0f - (float)i / M);
			addVec2(texcoords, (float)(j + 1) / N, 1.0f - (float)(i + 1) / M);
			addVec2(texcoords, (float)j / N, 1.0f - (float)(i + 1) / M);			

			numVertices += 3;
		}
	}

	vertices.insert(vertices.end(), normals.begin(), normals.end());
	vertices.insert(vertices.end(), texcoords.begin(), texcoords.end());

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	GLuint sphereVao = 0;
	glGenVertexArrays(1, &sphereVao);
	glBindVertexArray(sphereVao);
	glEnableVertexAttribArray(0);	
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4 * 2));
	
	glBindVertexArray(0);

	return Mesh(sphereVao, numVertices);
}

Mesh Mesh::makeCube(float size)
{
	std::vector<float> vertices;	
	std::vector<float> normals;
	std::vector<float> texcoords;

	//front 1
	addVec3(vertices, size, -size, size);
	addVec3(vertices, size, size, size);
	addVec3(vertices, size, size, -size);

	addVec3(normals, 1.0, 0.0, 0.0);
	addVec3(normals, 1.0, 0.0, 0.0);
	addVec3(normals, 1.0, 0.0, 0.0);

	addVec2(texcoords, 0.0, 1.0);
	addVec2(texcoords, 1.0, 1.0);
	addVec2(texcoords, 1.0, 0.0);

	//front 2
	addVec3(vertices, size, -size, size);
	addVec3(vertices, size, size, -size);
	addVec3(vertices, size, -size, -size);
	
	addVec3(normals, 1.0, 0.0, 0.0);
	addVec3(normals, 1.0, 0.0, 0.0);
	addVec3(normals, 1.0, 0.0, 0.0);

	addVec2(texcoords, 0.0, 1.0);
	addVec2(texcoords, 1.0, 0.0);
	addVec2(texcoords, 0.0, 0.0);

	//left 1
	addVec3(vertices, -size, -size, size);
	addVec3(vertices, size, -size, size);
	addVec3(vertices, size, -size, -size);

	addVec3(normals, 0.0, -1.0, 0.0);
	addVec3(normals, 0.0, -1.0, 0.0);
	addVec3(normals, 0.0, -1.0, 0.0);	

	addVec2(texcoords, 0.0, 1.0);
	addVec2(texcoords, 1.0, 1.0);
	addVec2(texcoords, 1.0, 0.0);

	//left 2
	addVec3(vertices, -size, -size, size);
	addVec3(vertices, size, -size, -size);
	addVec3(vertices, -size, -size, -size);

	addVec3(normals, 0.0, -1.0, 0.0);
	addVec3(normals, 0.0, -1.0, 0.0);
	addVec3(normals, 0.0, -1.0, 0.0);

	addVec2(texcoords, 0.0, 1.0);
	addVec2(texcoords, 1.0, 0.0);
	addVec2(texcoords, 0.0, 0.0);

	//top 1
	addVec3(vertices, -size, size, size);
	addVec3(vertices, size, size, size);
	addVec3(vertices, size, -size, size);

	addVec3(normals, 0.0, 0.0, 1.0);
	addVec3(normals, 0.0, 0.0, 1.0);
	addVec3(normals, 0.0, 0.0, 1.0);

	addVec2(texcoords, 0.0, 1.0);
	addVec2(texcoords, 1.0, 1.0);
	addVec2(texcoords, 1.0, 0.0);

	//top 2
	addVec3(vertices, -size, size, size);
	addVec3(vertices, -size, -size, size);
	addVec3(vertices, size, -size, size);

	addVec3(normals, 0.0, 0.0, 1.0);
	addVec3(normals, 0.0, 0.0, 1.0);
	addVec3(normals, 0.0, 0.0, 1.0);

	addVec2(texcoords, 0.0, 1.0);
	addVec2(texcoords, 0.0, 0.0);
	addVec2(texcoords, 1.0, 0.0);

	//back 1
	addVec3(vertices, -size, -size, size);
	addVec3(vertices, -size, size, -size);
	addVec3(vertices, -size, size, size);

	addVec3(normals, -1.0, 0.0, 0.0);
	addVec3(normals, -1.0, 0.0, 0.0);
	addVec3(normals, -1.0, 0.0, 0.0);

	addVec2(texcoords, 0.0, 1.0);
	addVec2(texcoords, 1.0, 0.0);
	addVec2(texcoords, 1.0, 1.0);

	//back 2
	addVec3(vertices, -size, -size, size);
	addVec3(vertices, -size, -size, -size);
	addVec3(vertices, -size, size, -size);

	addVec3(normals, -1.0, 0.0, 0.0);
	addVec3(normals, -1.0, 0.0, 0.0);
	addVec3(normals, -1.0, 0.0, 0.0);

	addVec2(texcoords, 0.0, 1.0);
	addVec2(texcoords, 0.0, 0.0);
	addVec2(texcoords, 1.0, 0.0);

	//right 1
	addVec3(vertices, -size, size, size);
	addVec3(vertices, size, size, -size);
	addVec3(vertices, size, size, size);

	addVec3(normals, 0.0, 1.0, 0.0);
	addVec3(normals, 0.0, 1.0, 0.0);
	addVec3(normals, 0.0, 1.0, 0.0);

	addVec2(texcoords, 0.0, 1.0);
	addVec2(texcoords, 1.0, 0.0);
	addVec2(texcoords, 1.0, 1.0);

	//right 2
	addVec3(vertices, -size, size, size);
	addVec3(vertices, -size, size, -size);
	addVec3(vertices, +size, size, -size);

	addVec3(normals, 0.0, 1.0, 0.0);
	addVec3(normals, 0.0, 1.0, 0.0);
	addVec3(normals, 0.0, 1.0, 0.0);

	addVec2(texcoords, 0.0, 1.0);
	addVec2(texcoords, 0.0, 0.0);
	addVec2(texcoords, 1.0, 0.0);

	//bottom 1
	addVec3(vertices, -size, size, -size);
	addVec3(vertices, size, -size, -size);
	addVec3(vertices, size, size, -size);

	addVec3(normals, 0.0, 0.0, -1.0);
	addVec3(normals, 0.0, 0.0, -1.0);
	addVec3(normals, 0.0, 0.0, -1.0);

	addVec2(texcoords, 0.0, 1.0);
	addVec2(texcoords, 1.0, 0.0);
	addVec2(texcoords, 1.0, 1.0);

	//bottom 2
	addVec3(vertices, -size, size, -size);
	addVec3(vertices, size, -size, -size);
	addVec3(vertices, -size, -size, -size);

	addVec3(normals, 0.0, 0.0, -1.0);
	addVec3(normals, 0.0, 0.0, -1.0);
	addVec3(normals, 0.0, 0.0, -1.0);

	addVec2(texcoords, 0.0, 1.0);
	addVec2(texcoords, 1.0, 0.0);
	addVec2(texcoords, 0.0, 0.0);

	vertices.insert(vertices.end(), normals.begin(), normals.end());
	vertices.insert(vertices.end(), texcoords.begin(), texcoords.end());

	int numVertices = 36;

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	GLuint cubeVao = 0;
	glGenVertexArrays(1, &cubeVao);
	glBindVertexArray(cubeVao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4 * 2));

	glBindVertexArray(0);
	
	return Mesh(cubeVao, numVertices);
}

Mesh Mesh::makeYZPlane(float size)
{
	std::vector<float> vertices;	
	std::vector<float> normals;
	std::vector<float> texcoords;

	//front 1
	addVec3(vertices, 0.0, -size, size);
	addVec3(vertices, 0.0, size, size);
	addVec3(vertices, 0.0, size, -size);

	addVec3(normals, 1.0, 0.0, 0.0);
	addVec3(normals, 1.0, 0.0, 0.0);
	addVec3(normals, 1.0, 0.0, 0.0);

	addVec2(texcoords, 0.0, 1.0);
	addVec2(texcoords, 1.0, 1.0);
	addVec2(texcoords, 1.0, 0.0);

	//front 2
	addVec3(vertices, 0.0, -size, size);
	addVec3(vertices, 0.0, size, -size);
	addVec3(vertices, 0.0, -size, -size);
	
	addVec3(normals, 1.0, 0.0, 0.0);
	addVec3(normals, 1.0, 0.0, 0.0);
	addVec3(normals, 1.0, 0.0, 0.0);

	addVec2(texcoords, 0.0, 1.0);
	addVec2(texcoords, 1.0, 0.0);
	addVec2(texcoords, 0.0, 0.0);

	vertices.insert(vertices.end(), normals.begin(), normals.end());
	vertices.insert(vertices.end(), texcoords.begin(), texcoords.end());

	int numVertices = 6;

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	GLuint planeVao = 0;
	glGenVertexArrays(1, &planeVao);
	glBindVertexArray(planeVao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4 * 2));

	glBindVertexArray(0);	

	return Mesh(planeVao, numVertices);
}

Mesh Mesh::makeScreenAlignedQuad()
{
	std::vector<float> vertices;	

	//front 1
	addVec3(vertices, -1.0, 1.0, 0.0);
	addVec3(vertices, 1.0, 1.0, 0.0);
	addVec3(vertices, 1.0, -1.0, 0.0);

	//front 2
	addVec3(vertices, -1.0, 1.0, 0.0);
	addVec3(vertices, 1.0, -1.0, 0.0);
	addVec3(vertices, -1.0, -1.0, 0.0);
	
	int numVertices = 6;

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(0);

	return Mesh(vao, numVertices);
}

Mesh Mesh::makeGroundPlane(float size, float numTiles)
{
	std::vector<float> vertices;	
	std::vector<float> normals;
	std::vector<float> texcoords;

	//front 1
	addVec3(vertices, -size, size, 0.0);
	addVec3(vertices, size, size, 0.0);
	addVec3(vertices, size, -size, 0.0);

	addVec3(normals, 0.0, 0.0, 1.0);
	addVec3(normals, 0.0, 0.0, 1.0);
	addVec3(normals, 0.0, 0.0, 1.0);

	addVec2(texcoords, -numTiles, numTiles);
	addVec2(texcoords, numTiles, numTiles);
	addVec2(texcoords, numTiles, -numTiles);

	//front 2
	addVec3(vertices, -size, size, 0.0);
	addVec3(vertices, size, -size, 0.0);
	addVec3(vertices, -size, -size, 0.0);

	addVec3(normals, 0.0, 0.0, 1.0);
	addVec3(normals, 0.0, 0.0, 1.0);
	addVec3(normals, 0.0, 0.0, 1.0);

	addVec2(texcoords, -numTiles, numTiles);
	addVec2(texcoords, numTiles, -numTiles);
	addVec2(texcoords, -numTiles, -numTiles);

	vertices.insert(vertices.end(), normals.begin(), normals.end());
	vertices.insert(vertices.end(), texcoords.begin(), texcoords.end());

	int numVertices = 6;

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	GLuint chessVao = 0;
	glGenVertexArrays(1, &chessVao);
	glBindVertexArray(chessVao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4 * 2));

	glBindVertexArray(0);

	return Mesh(chessVao, numVertices);
}


Mesh Mesh::loadFromFile(std::string filename)
{
	//const struct aiScene* scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	const struct aiScene* scene = aiImportFile(filename.c_str(), aiProcess_Triangulate);

	if (!scene)
	{
		std::cerr << aiGetErrorString() << std::endl;
		return Mesh();
	}

	if (scene->mNumMeshes == 0)
	{
		std::cerr << "There is no meshes in file " << filename << std::endl;;
		return Mesh();
	}

	const struct aiMesh* mesh = scene->mMeshes[0];

	if (!mesh->HasPositions())
	{
		std::cerr << "This demo does not support meshes without positions\n";
		return Mesh();
	}

	if (!mesh->HasNormals())
	{
		std::cerr << "This demo does not support meshes without normals\n";
		return Mesh();
	}
		
	if (!mesh->HasTextureCoords(0))
	{
		std::cerr << "This demo does not support meshes without texcoords for texture unit 0\n";
		return Mesh();
	}

	int numVertices = mesh->mNumVertices;

	std::vector<float> vertices(numVertices * 3);	
	std::vector<float> normals(numVertices * 3);
	std::vector<float> texcoords(numVertices * 2);
	
	for (int i = 0; i < numVertices; i++)
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

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(numVertices * 3 * 4 * 2));

	glBindVertexArray(0);	

	return Mesh(vao, numVertices);
}