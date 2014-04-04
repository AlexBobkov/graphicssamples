#version 330

const vec3 luminance = vec3(0.3, 0.59, 0.11);

uniform sampler2D tex;
uniform sampler2D bloomTex;
uniform sampler2D blurTex;
uniform sampler2D depthTex;
uniform float exposure;

uniform mat4 projMatrixInverse;

uniform float focalDistance;
uniform float focalRange;

in vec2 interpTc; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	//vec3 depthColor = texture(depthTex, interpTc).rgb;
	//vec3 normCoords = vec3(interpTc * 2.0 - 1.0, depthColor.z * 2.0 - 1.0);
	//vec4 pos = projMatrixInverse * vec4(normCoords, 1.0);
	//pos.xyz /= pos.w; //cam space

	//vec3 sharpColor = texture(tex, interpTc).rgb;
	//vec3 blurColor = texture(blurTex, interpTc).rgb;
	//vec3 texColor = mix(sharpColor, blurColor, clamp(abs(focalDistance + pos.z) / focalRange, 0.0, 1.0));
	
	vec3 texColor = texture(tex, interpTc).rgb;

	vec3 bloomColor = texture(bloomTex, interpTc).rgb;

	vec3 color = texColor + bloomColor * 5.0;

	fragColor.rgb = vec3(1.0) - exp(-exposure * color);

	//float l = dot(luminance, texColor);
	//float scale = l / (1.0 + l);
	//fragColor.rgb = texColor * scale;	

	fragColor.a = 1.0;
}
