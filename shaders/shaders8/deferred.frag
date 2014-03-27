#version 330

uniform sampler2D normalsTex;
uniform sampler2D diffuseTex;
uniform sampler2D depthTex;

uniform mat4 projMatrixInverse;

uniform vec3 ambientColor; //цвет окружающего света (аппроксимация множественных переотражений)
uniform vec3 diffuseColor; //цвет источника света
uniform vec3 specularColor; //бликовый цвет источника света

const float shininessFactor = 200.0; //блеск (свойство материала, влияет на размер блика)

in vec4 lightPosCamSpace; //положение источника света в системе координат камеры (интерполировано между вершинами треугольника)
in vec2 interpTc; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{	
	vec3 diffuseMaterial = texture(diffuseTex, interpTc).rgb;

	vec3 normalColor = texture(normalsTex, interpTc).rgb;	
	vec3 normal = normalColor * 2.0 - 1.0;
	
	vec3 depthColor = texture(depthTex, interpTc).rgb;
	vec3 normCoords = vec3(interpTc * 2.0 - 1.0, depthColor.z * 2.0 - 1.0);
	vec4 pos = projMatrixInverse * vec4(normCoords, 1.0);
	pos.xyz /= pos.w;

	vec3 lightDirCamSpace = lightPosCamSpace.xyz - pos.xyz; //направление на источник света
	lightDirCamSpace = normalize(lightDirCamSpace); //нормализуем направление
			    
    float cosAngIncidence = dot(normal, lightDirCamSpace); //интенсивность диффузного света
    cosAngIncidence = clamp(cosAngIncidence, 0, 1);

	vec3 viewDirection = normalize(-pos.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))
		
	vec3 halfAngle = normalize(lightDirCamSpace + viewDirection); //биссектриса между направлениями на камеру и на источник света
	float blinnTerm = dot(normal, halfAngle); //интенсивность бликового освещения по Блинну
	blinnTerm = clamp(blinnTerm, 0, 1);
	blinnTerm = cosAngIncidence != 0.0 ? blinnTerm : 0.0;
	blinnTerm = pow(blinnTerm, shininessFactor);  //регулируем размер блика

	//результирующий цвет
    vec3 color = diffuseMaterial * ambientColor + diffuseMaterial * diffuseColor * cosAngIncidence + specularColor * blinnTerm;

	fragColor = vec4(color, 1.0);

	//fragColor = vec4(diffuseMaterial, 1.0);
	//fragColor = vec4(normalColor, 1.0);
	//fragColor = vec4(depthColor, 1.0);
	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
