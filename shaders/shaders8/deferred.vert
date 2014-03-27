#version 330

layout(location = 0) in vec3 vp; //координаты вершины в локальной системе координат

uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform vec4 lightPos; //положение источника света в мировой системе координат (для точечного источника)

out vec2 interpTc; //выходные текстурные координаты
out vec4 lightPosCamSpace; //положение источника света в системе координат камеры

void main()
{	
	gl_Position = vec4(vp, 1.0);

	lightPosCamSpace = viewMatrix * lightPos; //преобразование положения источника света в систему координат камеры

	interpTc = (vp.xy + 1.0) * 0.5;
}
