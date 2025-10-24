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
		for (Particle* p : particles)
		{
			p->changeAcceleration(Vector3D(0, -9.8f, 0)); // Gravity
		}
	}
	void integrateAll(double t)
	{
		for (Particle* p : particles)
		{
			p->integrate(t);
		}
	}
};

