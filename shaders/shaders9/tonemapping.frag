#version 330

const vec3 luminance = vec3(0.3, 0.59, 0.11);

uniform sampler2D tex; //основная текстура
uniform sampler2D bloomTex; //текстура с размытиыми яркими точками (больше заданного порога)

uniform sampler2D blurTex; //размытая текстура для эффекта Depth of Field

uniform sampler2D depthTex; //карта глубин для реконструкции положения точки в системе координат виртуальной камеры

uniform float exposure; //параметр алгоритма Tone Mapping

uniform mat4 projMatrixInverse;

//параметры алгоритма Depth of Field
uniform float focalDistance;
uniform float focalRange;

in vec2 interpTc; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
#if 0 //код для алгоритма Depth of Field
	vec3 depthColor = texture(depthTex, interpTc).rgb;
	vec3 normCoords = vec3(interpTc * 2.0 - 1.0, depthColor.z * 2.0 - 1.0);
	vec4 pos = projMatrixInverse * vec4(normCoords, 1.0);
	pos.xyz /= pos.w; //cam space

	vec3 sharpColor = texture(tex, interpTc).rgb;
	vec3 blurColor = texture(blurTex, interpTc).rgb;
	vec3 texColor = mix(sharpColor, blurColor, clamp(abs(focalDistance + pos.z) / focalRange, 0.0, 1.0));
#else	
	vec3 texColor = texture(tex, interpTc).rgb;
#endif

	vec3 bloomColor = texture(bloomTex, interpTc).rgb;

	vec3 color = texColor + bloomColor * 5.0;

	fragColor.rgb = vec3(1.0) - exp(-exposure * color); //tone mapping: HDR to LDR
	fragColor.a = 1.0;
}
