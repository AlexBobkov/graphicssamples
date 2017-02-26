/*
Рисует фрактал Мандельброта
*/

#version 330

in vec2 position;

out vec4 fragColor;

void main()
{
    float MaxIterations = 100;
    float Zoom = 2.0;
    float Xcenter = -1.0;
    float Ycenter = 0.0;
    vec3 InnerColor = vec3(0.0, 0.0, 0.0);
    vec3 OuterColor1 = vec3(1.0, 1.0, 0.0);
    vec3 OuterColor2 = vec3(0.0, 0.0, 1.0);

    float real  = position.x * Zoom + Xcenter;
    float imag  = position.y * Zoom + Ycenter;
    float Creal = real;   // Change this line...
    float Cimag = imag;   // ...and this one to get a Julia set

    float r2 = 0.0;
    float iter;

    for (iter = 0.0; iter < MaxIterations && r2 < 4.0; ++iter)
    {
        float tempreal = real;

        real = (tempreal * tempreal) - (imag * imag) + Creal;
        imag = 2.0 * tempreal * imag + Cimag;
        r2   = (real * real) + (imag * imag);
    }

    // Base the color on the number of iterations

    vec3 color;

    if (r2 < 4.0)
        color = InnerColor;
    else
        color = mix(OuterColor1, OuterColor2, fract(iter * 0.05));

    fragColor = vec4(color, 1.0);
}
