#version 330

uniform float deltaTime;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexVelocity;
layout(location = 2) in float oldParticleTime;

out vec3 position;
out vec3 velocity;
out float particleTime;

const float lifeTime = 15.0f;

void main()
{	
	particleTime = oldParticleTime + deltaTime;

	if (particleTime > 0.0)
	{
		velocity = vertexVelocity + vec3(0.0, 0.0, -1.0) * deltaTime;

		position = vertexPosition + velocity * deltaTime;
		
		if (position.z < 0.0)
		{
			position.z *= -1;
			velocity.z *= -0.75;
		}

		if (particleTime > lifeTime)
		{
			particleTime -= lifeTime;
			
			position.z = 0.0;
			vec3 dir = normalize(position);
			
			position = dir * 0.25;
			velocity = vec3(dir.xy * 0.2, 1.0) * 3.0;
		}
	}
	else
	{
		position = vertexPosition;
		velocity = vertexVelocity;
	}
}
