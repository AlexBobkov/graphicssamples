#version 330

in vec4 theColor;

out vec4 frag_colour;

void main()
{
	frag_colour = theColor;
}