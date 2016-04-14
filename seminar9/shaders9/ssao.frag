#version 330

uniform sampler2D depthTex; //Карта глубин
uniform sampler2D rotateTex; //Текстура со случайными значениям

uniform mat4 projMatrix;
uniform mat4 projMatrixInverse;

uniform float attBias;
uniform float attScale;
uniform float radius;

in vec2 texCoord; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

//8 лучей из центра куба к его вершинам
vec4 rndTable[8] = vec4[8] 
(
	vec4 ( -0.5, -0.5, -0.5, 0.0 ),
	vec4 (  0.5, -0.5, -0.5, 0.0 ),
	vec4 ( -0.5,  0.5, -0.5, 0.0 ),
	vec4 (  0.5,  0.5, -0.5, 0.0 ),
	vec4 ( -0.5, -0.5,  0.5, 0.0 ),
	vec4 (  0.5, -0.5,  0.5, 0.0 ),
	vec4 ( -0.5,  0.5,  0.5, 0.0 ),
	vec4 (  0.5,  0.5,  0.5, 0.0 )
);

void main()
{
	float mainFragDepth = texture(depthTex, texCoord).r;
	vec3 normCoords = vec3(texCoord, mainFragDepth) * 2.0 - 1.0;
	vec4 pos = projMatrixInverse * vec4(normCoords, 1.0);
	pos.xyz /= pos.w; //положение точки в системе координат виртуальной камеры

	//случайный вектор читаем из текстуры и интерпретируем, как нормаль для случайной плоскости
	vec3 rotateColor = texture(rotateTex, gl_FragCoord.xy * 0.25).rgb;	
	vec3 plane = rotateColor * 2.0 - 1.0;

	float att = 0.0;
	for (int i = 0; i < 8; i++)
	{
		vec3 sample = reflect(rndTable[i].xyz, plane); //отражаем лучи от случайной плоскости. Получаем случайные лучи

		vec4 sampleP = projMatrix * vec4(pos.xyz + sample * radius, 1.0);
		sampleP.xyz /= sampleP.w;
		sampleP.xyz = sampleP.xyz * 0.5 + 0.5;
		
		float fragDepth = sampleP.z; //глубина точки, куда указывает луч
		float shadowDepth = texture(depthTex, sampleP.xy).r;

		if (fragDepth < shadowDepth) //проверяем, точка находится внутри геометрии или снаружи
		{
			att += 1.0;
		}
	}
	
	att = clamp((att / 8.0 + attBias) * attScale, 0.0, 1.0);
	
	if (mainFragDepth == 1.0) //спец. случай для точек на бесконечности (фон)
	{
		att = 1.0;
	}

	fragColor = vec4(att, att, att, 1.0);
}
