#version 330

const vec2 corners[4] = vec2[](vec2(0.0, 1.0), vec2(0.0, 0.0), vec2(1.0, 1.0), vec2(1.0, 0.0));

uniform mat4 projectionMatrix;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in float ratioVert[];

out vec2 texCoord;
out float ratio;

const float size = 0.3;

void main()
{
	float curSize = size * (ratioVert[0] + 1.0);

	vec4 eyePos = gl_in[0].gl_Position;
	eyePos.xy += curSize * vec2(-0.5, 0.5);
	gl_Position = projectionMatrix * eyePos;

	ratio = ratioVert[0];
	texCoord = vec2(0.0, 1.0);

	EmitVertex();
	
	eyePos = gl_in[0].gl_Position;
	eyePos.xy += curSize * vec2(-0.5, -0.5);
	gl_Position = projectionMatrix * eyePos;

	ratio = ratioVert[0];
	texCoord = vec2(0.0, 0.0);

	EmitVertex();
	
	eyePos = gl_in[0].gl_Position;
	eyePos.xy += curSize * vec2(0.5, 0.5);
	gl_Position = projectionMatrix * eyePos;

	ratio = ratioVert[0];
	texCoord = vec2(1.0, 1.0);

	EmitVertex();
	
	eyePos = gl_in[0].gl_Position;
	eyePos.xy += curSize * vec2(0.5, -0.5);
	gl_Position = projectionMatrix * eyePos;

	ratio = ratioVert[0];
	texCoord = vec2(1.0, 0.0);

	EmitVertex();

	EndPrimitive();
}
