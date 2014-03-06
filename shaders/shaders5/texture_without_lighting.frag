#version 330

uniform sampler2D diffuseTex;

in vec2 interpTc; //���������� ���������� (��������������� ����� ��������� ������������)

out vec4 fragColor; //�������� ���� ���������

void main()
{
	vec3 texColor = texture(diffuseTex, interpTc).rgb;

	fragColor = vec4(texColor, 1.0); //������ ��������
	//fragColor = vec4(interpTc, 0.0, 1.0); //������ ��������
}