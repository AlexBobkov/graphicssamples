#version 330

uniform vec3 ambientColor; //цвет окружающего света (аппроксимация множественных переотражений)
uniform vec3 diffuseColor; //цвет источника света
uniform vec3 specularColor; //бликовый цвет источника света
uniform float shininessFactor; //блеск (свойство материала, влияет на размер блика)
uniform vec3 material; //свойство материала (коэффициенты отражения для 3х компонент цвета)

in vec3 normalCamSpace; //нормаль в системе координат камеры (интерполирована между вершинами треугольника)
in vec4 lightPosCamSpace; //положение источника света в системе координат камеры (интерполировано между вершинами треугольника)
in vec4 posCamSpace; //координаты вершины в системе координат камеры (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	vec3 lightDirCamSpace = normalize(lightPosCamSpace.xyz - posCamSpace.xyz); //направление на источник света

	vec3 normal = normalize(normalCamSpace); //нормализуем нормаль после интерполяции
				    
    float cosAngIncidence = dot(normal, lightDirCamSpace); //интенсивность диффузного света
    cosAngIncidence = clamp(cosAngIncidence, 0, 1);
    
	vec3 viewDirection = normalize(-posCamSpace.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))
		
	vec3 halfAngle = normalize(lightDirCamSpace + viewDirection); //биссектриса между направлениями на камеру и на источник света
	float blinnTerm = dot(normal, halfAngle); //интенсивность бликового освещения по Блинну
	blinnTerm = clamp(blinnTerm, 0, 1);
	blinnTerm = cosAngIncidence != 0.0 ? blinnTerm : 0.0;
	blinnTerm = pow(blinnTerm, shininessFactor); //регулируем размер блика

    vec3 color = material * ambientColor + material * diffuseColor * cosAngIncidence + specularColor * blinnTerm; //результирующий цвет

	fragColor = vec4(color, 1.0); //просто копируем
}
