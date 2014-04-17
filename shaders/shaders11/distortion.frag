#version 330

uniform sampler2D tex;
uniform vec2 LensCenter;
uniform vec2 ScreenCenter;
uniform vec2 Scale;
uniform vec2 ScaleIn;
uniform vec4 HmdWarpParam;

in vec2 interpTc; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

// Scales input texture coordinates for distortion.
vec2 HmdWarp(vec2 texCoord)
{
	vec2 theta = (texCoord - LensCenter) * ScaleIn; // Scales texture coordinates to [-1, 1]
	float rSq = theta.x * theta.x + theta.y * theta.y;
	vec2 rvector= theta * (	HmdWarpParam.x +
							HmdWarpParam.y * rSq +
							HmdWarpParam.z * rSq * rSq +
							HmdWarpParam.w * rSq * rSq * rSq);
	return LensCenter + Scale * rvector;
}

void main()
{
	//vec3 texColor = texture(tex, interpTc).rgb;	

	//fragColor = vec4(texColor, 1.0); //просто копируем

	vec2 tc = HmdWarp(interpTc);

	if (any(bvec2(clamp(tc, ScreenCenter - vec2(0.5, 0.5), ScreenCenter + vec2(0.5, 0.5)) - tc))) {
		gl_FragColor = vec4(0, 0, 0, 0);
	} else {
		gl_FragColor = texture2D(tex, tc);
	}
}