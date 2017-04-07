#version 330

uniform sampler2D tex;

in vec2 texCoord; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	vec2 size = textureSize(tex, 0);
    vec2 dx = vec2(0.0, 1.0 / size.y); //шаг по горизонтали
    vec2 sdx = dx;
    vec4 sum = texture(tex, texCoord) * 0.134598;

	//Размытие по вертикали по Гауссу
    sum += (texture(tex, texCoord + sdx) + texture(tex, texCoord - sdx)) * 0.127325;
    sdx += dx;
    sum += (texture(tex, texCoord + sdx) + texture(tex, texCoord - sdx)) * 0.107778;
    sdx += dx;
    sum += (texture(tex, texCoord + sdx) + texture(tex, texCoord - sdx)) * 0.081638;
    sdx += dx;
    sum += (texture(tex, texCoord + sdx) + texture(tex, texCoord - sdx)) * 0.055335;
    sdx += dx;
    sum += (texture(tex, texCoord + sdx) + texture(tex, texCoord - sdx)) * 0.033562;
    sdx += dx;
    sum += (texture(tex, texCoord + sdx) + texture(tex, texCoord - sdx)) * 0.018216;
    sdx += dx;
    sum += (texture(tex, texCoord + sdx) + texture(tex, texCoord - sdx)) * 0.008847;
    sdx += dx;

    fragColor = sum;
}
