#version 330

uniform sampler2D tex; //основная текстура
uniform sampler2D blurTex; //размытая текстура для эффекта Depth of Field

uniform sampler2D depthTex; //карта глубин для реконструкции положения точки в системе координат виртуальной камеры

uniform mat4 projMatrixInverse;

//параметры алгоритма Depth of Field
uniform float focalDistance;
uniform float focalRange;

in vec2 texCoord; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	float fragDepth = texture(depthTex, texCoord).r;
	vec3 normCoords = vec3(texCoord, fragDepth) * 2.0 - 1.0;
	vec4 pos = projMatrixInverse * vec4(normCoords, 1.0);
	pos.xyz /= pos.w; //cam space

	vec3 sharpColor = texture(tex, texCoord).rgb;
	vec3 blurColor = texture(blurTex, texCoord).rgb;
	
	float alpha = clamp(abs(focalDistance + pos.z) / focalRange, 0.0, 1.0);
	vec3 color = mix(sharpColor, blurColor, alpha);

	fragColor = vec4(color, 1.0);
}
