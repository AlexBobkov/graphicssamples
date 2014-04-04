#version 330

uniform sampler2D depthTex;
uniform sampler2D rotateTex;

uniform mat4 projMatrix;
uniform mat4 projMatrixInverse;

in vec2 interpTc; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

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

const float attBias = 0.5;
const float attScale = 1.0;

void main()
{
	vec3 depthColor = texture(depthTex, interpTc).rgb;
	vec3 normCoords = vec3(interpTc * 2.0 - 1.0, depthColor.z * 2.0 - 1.0);
	vec4 pos = projMatrixInverse * vec4(normCoords, 1.0);
	pos.xyz /= pos.w; //cam space

	vec3 rotateColor = texture(rotateTex, gl_FragCoord.xy * 0.25).rgb;	
	vec3 plane = 2.0 * rotateColor - 1.0;
	float att = 0.0;

	for (int i = 0; i < 8; i++)
	{
		vec3 sample = reflect(rndTable[i].xyz, plane); //cam space

		vec4 sampleP = projMatrix * vec4(pos.xyz + sample / 10.0, 1.0);
		sampleP.xyz /= sampleP.w;
		sampleP.xyz = sampleP.xyz * 0.5 + 0.5;
		
		float fragDepth = sampleP.z;
		float shadowDepth = texture(depthTex, sampleP.xy).z;

		if (fragDepth < shadowDepth)
		{
			att += 1.0;
		}

		//float   dz = max ( zSample - z, 0.0 ) * 30.0;
		//att += 1.0 / ( 1.0 + dz*dz );
	}

	att = clamp((att / 8.0 + attBias) * attScale, 0.0, 1.0);
	//att /= 8.0;

	if (depthColor.r == 1.0)
	{
		att = 1.0;
	}

	fragColor = vec4(att, att, att, 1.0);
	//fragColor = vec4(rotateColor, 1.0);
}
