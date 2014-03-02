#version 330

uniform vec3 ambientColor; //цвет окружающего света (аппроксимация множественных переотражений)
uniform vec3 diffuseColor; //цвет источника света

in vec3 normalCamSpace; //нормаль в системе координат камеры (интерполирована между вершинами треугольника)
in vec4 lightPosCamSpace; //положение источника света в системе координат камеры (интерполировано между вершинами треугольника)
in vec4 posCamSpace; //координаты вершины в системе координат камеры (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	vec3 lightDirCamSpace = normalize(lightPosCamSpace.xyz - posCamSpace.xyz); //направление на источник света
				    
    float cosAngIncidence = dot(normalize(normalCamSpace), lightDirCamSpace.xyz); //интенсивность диффузного света
    cosAngIncidence = clamp(cosAngIncidence, 0, 1);
    
    vec3 color = ambientColor + diffuseColor * cosAngIncidence;	//результирующий цвет

	fragColor = vec4(color, 1.0); //просто копируем
}