#version 330

//стандартные матрицы для преобразования координат
uniform mat4 modelMatrix; //из локальной в мировую
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

//матрица для преобразования нормалей из локальной системы координат в систему координат камеры
uniform mat3 normalToCameraMatrix;

uniform vec4 lightDir; //направление на источник света в мировой системе координат (для направленного источника)
uniform vec3 ambientColor; //цвет окружающего света (аппроксимация множественных переотражений)
uniform vec3 diffuseColor; //цвет источника света

layout(location = 0) in vec3 vp; //координаты вершины в локальной системе координат
layout(location = 1) in vec3 normal; //нормаль в локальной системе координат

out vec3 interpColor; //цвет вершины

void main()
{
	vec3 normalCamSpace = normalize(normalToCameraMatrix * normal); //нормаль - из локальной в систему координат камеры
	vec4 lightDirCamSpace = viewMatrix * lightDir; //направление на источник света - из мировой в систему координат камеры 
				    
    float cosAngIncidence = dot(normalCamSpace, lightDirCamSpace.xyz); //скалярное произведение (косинус)
    cosAngIncidence = clamp(cosAngIncidence, 0, 1);
    
    interpColor = ambientColor + diffuseColor * cosAngIncidence; //цвет вершины
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.0);
}
