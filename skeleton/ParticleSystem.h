#include "Particle.h"
#include <vector>
#include "Vector3D.h"

#pragma once
class ParticleSystem
{
private:
	std::vector<Particle*> particles;
	//acceleration, spawn rate, distribution, life span, etc.
	Vector3D globalAcceleration;
	float spawnRate;
	float timeSinceLastSpawn;
	float particleLifeSpan;

	float distributionRadius;

	// Helper function to generate random float between min and max
	float randomFloat(float min, float max)
	{
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = max - min;
		float r = random * diff;
		return min + r;
	}

public:
	//constructor con parametros spawn rate, distribution radius, global acceleration etc.
	ParticleSystem(float spawnRate = 10.0f, float distributionRadius = 5.0f, Vector3D globalAcc = Vector3D(0, -9.8f, 0), float particleLifeSpan = 5.0f)
		: spawnRate(spawnRate), distributionRadius(distributionRadius), globalAcceleration(globalAcc), particleLifeSpan(particleLifeSpan), timeSinceLastSpawn(0.0f)
	{
	}
	ParticleSystem() {}
	~ParticleSystem() 
	{
		for (Particle* p : particles)
		{
			delete p;
		}
		particles.clear();
	}
	void addParticle(Particle* p)
	{
		particles.push_back(p);
	}

	void updateAll(double t)
	{
		//create new particles based on spawn rate
		timeSinceLastSpawn += t;
		while (timeSinceLastSpawn >= (1.0f /spawnRate))
		{
			//spawn new particle at random position within distribution radius
			float angle = randomFloat(0.0f, 2.0f * 3.14159f);
			float radius = randomFloat(0.0f, distributionRadius);
			Vector3D spawnPos = Vector3D(radius * cos(angle), 0, radius * sin(angle));
			//create new particle with initial velocity and global acceleration
			Particle* newParticle = new Particle(spawnPos, Vector3D(0, 0, 0), globalAcceleration, 1.0f);
			addParticle(newParticle);
			timeSinceLastSpawn -= (1.0f / spawnRate);
		}
	}
	void integrateAll(double t)
	{
		for (Particle* p : particles)
		{
			p->integrate(t);
		}
	}

	void clearDeadParticles()
	{
		for (Particle* p : particles)
		{
			if (!p->isAlive())
			{
				auto it = std::find(particles.begin(), particles.end(), p);
				if (it != particles.end())
				{
					delete p;
					particles.erase(it);
				}
			}
		}
	}
};

