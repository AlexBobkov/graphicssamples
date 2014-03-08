#version 330

uniform float time;

in vec4 theColor;
in vec4 thePosition;

out vec4 frag_colour;

void main()
{
	frag_colour = thePosition * 0.5 * (2.0 + sin(time));
}
