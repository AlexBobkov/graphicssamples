#version 330

uniform sampler2D diffuseTex;
uniform sampler2D specularTex;

uniform vec3 ambientColor; //���� ����������� ����� (������������� ������������� �������������)
uniform vec3 diffuseColor; //���� ��������� �����
uniform vec3 specularColor; //�������� ���� ��������� �����
uniform float shininessFactor; //����� (�������� ���������, ������ �� ������ �����)
uniform vec3 material; //�������� ��������� (������������ ��������� ��� 3� ��������� �����)

in vec3 normalCamSpace; //������� � ������� ��������� ������ (��������������� ����� ��������� ������������)
in vec4 lightPosCamSpace; //��������� ��������� ����� � ������� ��������� ������ (��������������� ����� ��������� ������������)
in vec4 posCamSpace; //���������� ������� � ������� ��������� ������ (��������������� ����� ��������� ������������)

in vec2 interpTc; //���������� ���������� (��������������� ����� ��������� ������������)

out vec4 fragColor; //�������� ���� ���������

void main()
{
	vec3 diffuseMaterial = texture(diffuseTex, interpTc).rgb;
	vec3 specularMaterial = texture(specularTex, interpTc).rgb;

	vec3 lightDirCamSpace = normalize(lightPosCamSpace.xyz - posCamSpace.xyz); //����������� �� �������� �����

	vec3 normal = normalize(normalCamSpace); //����������� ������� ����� ������������
				    
    float cosAngIncidence = dot(normal, lightDirCamSpace); //������������� ���������� �����
    cosAngIncidence = clamp(cosAngIncidence, 0, 1);
    
	vec3 viewDirection = normalize(-posCamSpace.xyz); //����������� �� ����������� ������ (��� ��������� � ����� (0.0, 0.0, 0.0))
		
	vec3 halfAngle = normalize(lightDirCamSpace + viewDirection); //����������� ����� ������������� �� ������ � �� �������� �����
	float blinnTerm = dot(normal, halfAngle); //������������� ��������� ��������� �� ������
	blinnTerm = clamp(blinnTerm, 0, 1);
	blinnTerm = cosAngIncidence != 0.0 ? blinnTerm : 0.0;
	blinnTerm = pow(blinnTerm, shininessFactor); //���������� ������ �����

    vec3 color = diffuseMaterial * ambientColor + diffuseMaterial * diffuseColor * cosAngIncidence + specularMaterial * specularColor * blinnTerm; //�������������� ����	

	fragColor = vec4(color, 1.0); //������ ��������
}