#version 330

in vec3 color;

out vec4 fragColor;

void main()
{
	fragColor = vec4((color.xyz + 1.0) * 0.5, 1.0);
}
