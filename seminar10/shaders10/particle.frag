#version 330

uniform sampler2D tex;

layout(location = 0) out vec4 fragColor; //выходной цвет фрагмента

in float ratio;

vec4 getColorFromPalette(float value)
{
	float alpha = 1.0;
	vec4 color;
	
	if (value < 0.25)
	{
		color = vec4(0.0, value * 4.0, 1.0, alpha);
	}
	else if (value < 0.5)
	{
		color = vec4(0.0, 1.0, (0.5 - value) * 4.0, alpha);
	}
	else if (value < 0.75)
	{
		color = vec4((value - 0.5) * 4.0, 1.0, 0.0, alpha);
	}
	else
	{
		color = vec4(1.0, (1.0 - value) * 4.0, 0.0, alpha);
	}
	return color;
}

void main()
{
	vec4 color = texture(tex, gl_PointCoord); //читаем из текстуры	
	//color *= getColorFromPalette(ratio);
	color.a *= (1.0 - ratio);

	fragColor = color;
	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
