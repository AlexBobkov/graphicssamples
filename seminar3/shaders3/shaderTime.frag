#version 330

uniform float time;

in vec4 theColor;

out vec4 frag_colour;

void main()
{
	vec4 colorOffset = vec4(0.5 * (1.0 + sin(time)), cos(gl_FragCoord.y), 0.0, 0.0);
	
	frag_colour = theColor + colorOffset;
}
