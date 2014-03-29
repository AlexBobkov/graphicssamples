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

/**
Содержит VAO для меша (полигональной 3д-модели) и количество вершин.
Также имеется несколько статических методов для создания конкретных мешей.
*/
class Mesh
{
public:
	Mesh();
	Mesh(GLuint vao, int numVertices);

	GLuint getVao() { return _vao; }
	int getNumVertices() { return _numVertices; }

	static Mesh makeSphere(float radius);	
	static Mesh makeCube(float size);
	static Mesh makeYZPlane(float size);
	static Mesh makeScreenAlignedQuad();
	static Mesh makeGroundPlane(float size, float numTiles);
	static Mesh loadFromFile(std::string filename);

protected:
	GLuint _vao;
	int _numVertices;	
};