/**
Повершинное освещение точечным источником света. Только окружающий и диффузный света
*/

#version 330

//стандартные матрицы для преобразования координат
uniform mat4 modelMatrix; //из локальной в мировую
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

//матрица для преобразования нормалей из локальной системы координат в систему координат камеры
uniform mat3 normalToCameraMatrix;

struct LightInfo
{
	vec3 pos; //положение источника света в мировой системе координат (для точечного источника)
	vec3 La; //цвет и интенсивность окружающего света
	vec3 Ld; //цвет и интенсивность диффузного света
};
uniform LightInfo light;

struct MaterialInfo
{	
	vec3 Ka; //коэффициент отражения окружающего света
	vec3 Kd; //коэффициент отражения диффузного света
};
uniform MaterialInfo material;

layout(location = 0) in vec3 vertexPosition; //координаты вершины в локальной системе координат
layout(location = 1) in vec3 vertexNormal; //нормаль в локальной системе координат

out vec3 color; //выходной цвет вершины

void main()
{
	vec3 normalCamSpace = normalize(normalToCameraMatrix * vertexNormal); //нормаль - из локальной в систему координат камеры

	vec4 posCamSpace = viewMatrix * modelMatrix * vec4(vertexPosition, 1.0); //координаты вершины из локальной в систему координат камеры
	vec4 lightPosCamSpace = viewMatrix * vec4(light.pos, 1.0); //положение источника света - из мировой в систему координат камеры 
	vec4 lightDirCamSpace = normalize(lightPosCamSpace - posCamSpace); 
				    
    float NdotL = max(dot(normalCamSpace, lightDirCamSpace.xyz), 0.0); //скалярное произведение (косинус)
        
    color = light.La * material.Ka + light.Ld * material.Kd * NdotL; //цвет вершины
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}
