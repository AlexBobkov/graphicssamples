#version 330

in vec4 theColor;

out vec4 frag_colour;

void main()
{
	//frag_colour = vec4(0.5, 0.0, 1.0, 1.0);
	frag_colour = theColor;
}