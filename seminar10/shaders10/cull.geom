#version 430

layout(points) in;
layout(points, max_vertices = 1) out;

uniform float time;

out vec3 position;

void main()
{
    //Здесь может быть произвольное условия отбрасывания
    if (sin(time + gl_in[0].gl_Position.y * 0.2) > 0.0)
    {
        position = gl_in[0].gl_Position.xyz;
    
        EmitVertex();        
        EndPrimitive();
    }
}
