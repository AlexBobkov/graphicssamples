#version 330

//G-буфер
uniform sampler2D normalsTex;
uniform sampler2D diffuseTex;
uniform sampler2D depthTex;

//карта теней
uniform sampler2D shadowTex;

//карта с ambient occlusion
uniform sampler2D ssaoTex;

//Обратные матрицы для главной камеры
uniform mat4 projMatrixInverse;
uniform mat4 viewMatrixInverse;

//стандартные матрицы для проектора
uniform mat4 lightViewMatrix; //из мировой в систему координат камеры
uniform mat4 lightProjectionMatrix; //из системы координат камеры в усеченные координаты
uniform mat4 lightScaleBiasMatrix;

uniform vec3 ambientColor; //цвет окружающего света (аппроксимация множественных переотражений)
uniform vec3 diffuseColor; //цвет источника света
uniform vec3 specularColor; //бликовый цвет источника света

uniform bool addShadow;
uniform bool addSSAO;

const float bias = 0.0005;

const float shininessFactor = 200.0; //блеск (свойство материала, влияет на размер блика)
const float attenuation = 0.01;

in vec4 lightPosCamSpace; //положение источника света в системе координат камеры (интерполировано между вершинами треугольника)
in vec2 interpTc; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{	
	vec3 diffuseMaterial = texture(diffuseTex, interpTc).rgb;

	vec3 normalColor = texture(normalsTex, interpTc).rgb;	
	vec3 normal = normalColor * 2.0 - 1.0;
		
	if (length(normalColor) < 0.1)
	{
		discard; //Отбрасываем части изображения, которые относятся к фону, а не 3д-моделям
		return;
	}

	vec3 depthColor = texture(depthTex, interpTc).rgb;
	vec3 normCoords = vec3(interpTc * 2.0 - 1.0, depthColor.z * 2.0 - 1.0);
	vec4 pos = projMatrixInverse * vec4(normCoords, 1.0);
	pos.xyz /= pos.w; //положение в системе координат виртуальной камеры

	//вычисляем текстурные координаты для теневой карты
	vec4 shadowTc = lightScaleBiasMatrix * lightProjectionMatrix * lightViewMatrix * viewMatrixInverse * vec4(pos.xyz, 1.0);	
	shadowTc.xyz /= shadowTc.w;

	float visibility = 1.0;
	if (addShadow)
	{
		float fragDepth = shadowTc.z; //глубина фрагмента в пространстве источника света
		float shadowDepth = texture(shadowTex, shadowTc.xy).z; //глубина ближайшего фрагмента в пространстве источника света

		if (fragDepth > shadowDepth)
		{
			visibility = 0.0;
		}
	}


	vec3 lightDirCamSpace = lightPosCamSpace.xyz - pos.xyz; //направление на источник света
	float lightDistance = length(lightDirCamSpace);
	lightDirCamSpace = normalize(lightDirCamSpace); //нормализуем направление

	float cosAngIncidence = dot(normal, lightDirCamSpace); //интенсивность диффузного света
	cosAngIncidence = clamp(cosAngIncidence, 0, 1);

	vec3 viewDirection = normalize(-pos.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))

	vec3 halfAngle = normalize(lightDirCamSpace + viewDirection); //биссектриса между направлениями на камеру и на источник света
	float blinnTerm = dot(normal, halfAngle); //интенсивность бликового освещения по Блинну
	blinnTerm = clamp(blinnTerm, 0, 1);
	blinnTerm = cosAngIncidence != 0.0 ? blinnTerm : 0.0;
	blinnTerm = pow(blinnTerm, shininessFactor);  //регулируем размер блика

	vec3 ambient = ambientColor;
	if (addSSAO)
	{
		ambient *= texture(ssaoTex, interpTc).rgb; //модифицируем ambient-компоненты освещения
	}

	//результирующий цвет
	vec3 color = diffuseMaterial * ambient + (visibility * diffuseMaterial * diffuseColor * cosAngIncidence + visibility * specularColor * blinnTerm) / (1.0 + attenuation * lightDistance * lightDistance);

	fragColor = vec4(color, 1.0);

	//fragColor = vec4(visibility, visibility, visibility, 1.0);
	//fragColor = vec4(diffuseMaterial * ambient, 1.0);
	//fragColor = vec4(diffuseMaterial, 1.0);
	//fragColor = vec4(normalColor, 1.0);
	//fragColor = vec4(depthColor, 1.0);
	//fragColor = vec4(texture(shadowTex, shadowTc.xyz), 1.0);
	//fragColor = vec4(interpTc, 0.0, 1.0);
	//fragColor = vec4(shadowTc.xy, 0.0, 1.0);
	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
