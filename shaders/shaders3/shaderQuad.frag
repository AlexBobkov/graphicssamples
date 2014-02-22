#version 330

in vec2 pos;

out vec4 frag_colour;

 


void main()
{
	frag_colour = vec4(1.0, 1.0, 0.0, 1.0);
	
	if (length(pos) > 0.5)
	{
		discard;
	}
}