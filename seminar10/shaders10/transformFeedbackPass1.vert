#version 330

uniform float deltaTime;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexVelocity;
layout(location = 2) in float oldParticleTime;

out vec3 position;
out vec3 velocity;
out float particleTime;

const float lifeTime = 3.0f;

void main()
{	
	particleTime = oldParticleTime + deltaTime;

	velocity = vertexVelocity + vec3(0.0, 0.0, -1.0) * deltaTime;

	position = vertexPosition + velocity * deltaTime;

	if (particleTime > lifeTime)
	{
		particleTime -= lifeTime;
		
		position.z = 0.0;
		velocity = vec3(position.x * 0.1, position.y * 0.1, 1.0);
	}
}
