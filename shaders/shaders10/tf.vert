#version 330

uniform float deltaTime;

layout(location = 0) in vec3 vp; //координаты вершины в локальной системе координат
layout(location = 1) in vec3 vel;
layout(location = 2) in float pTime;

out vec3 position;
out vec3 velocity;
out float particleTime;

const float lifeTime = 3.0f;

void main()
{	
	particleTime = pTime + deltaTime;

	velocity = vel + vec3(0.0, 0.0, -1.0) * deltaTime;

	position = vp + velocity * deltaTime;

	if (particleTime > lifeTime)
	{
		particleTime -= lifeTime;

		position.z = 0.0;
		velocity = vec3(position.x * 0.1, position.y * 0.1, 1.0);
	}
}
