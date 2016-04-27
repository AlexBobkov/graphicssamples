#version 430

layout(points) in;
layout(points, max_vertices = 1) out;

//стандартные матрицы для преобразования координат
uniform mat4 modelMatrix; //из локальной в мировую
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

out vec3 position;

void main()
{
    //Здесь может быть произвольное условия отбрасывания
    if (gl_in[0].gl_Position.y > 0.0)
    {
        position = gl_in[0].gl_Position.xyz;
        
        gl_Position = projectionMatrix * viewMatrix * modelMatrix * gl_in[0].gl_Position;
    
        EmitVertex();        
        EndPrimitive();
    }
}
