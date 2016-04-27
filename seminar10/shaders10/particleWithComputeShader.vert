#version 330

//стандартные матрицы для преобразования координат
uniform mat4 modelMatrix; //из локальной в мировую
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

layout(location = 0) in vec4 vertexPosition; //координаты вершины в локальной системе координат

void main()
{	
	//Вычисляем размер частицы с учётом эффекта перспективы
	//vec4 posCamSpace = viewMatrix * modelMatrix * vertexPosition;
	//gl_PointSize = 200.0 / (-posCamSpace.z);
	gl_PointSize = 2.0;
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;
}
