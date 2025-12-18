#include "Particle.h"
#include <vector>
#include <list>
#include "Vector3D.h"
#include <PxPhysicsAPI.h>
#include "ForceGenerator.h"
using namespace physx;

#pragma once
class ParticleSystem
{
private:
	std::list<Particle*> particles;
	std::vector<Particle*> dParticles;
	std::vector<ForceGenerator*> forceGenerators;

	Vector3D globalAcceleration;
	float spawnRate;
	float timeSinceLastSpawn;
	float particleLifeSpan;
	Vector3D position;
	float size;
	Vector4 color;

	// Parámetros para el área de spawn
	float spawnWidth;
	float spawnDepth;
	float spawnHeight;

	// Helper function to generate random float between min and max
	float randomFloat(float min, float max)
	{
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = max - min;
		float r = random * diff;
		return min + r;
	}

public:
	void addForceGenerator(ForceGenerator* fg)
	{
		forceGenerators.push_back(fg);
	}

	ParticleSystem(float spawnRate = 100.0f,
		float spawnWidth = 100.0f,
		float spawnDepth = 50.0f,
		float spawnHeight = 50.0f,
		Vector3D pos = Vector3D(0, 50, 0),
		Vector3D globalAcc = Vector3D(0, 0, 0),
		float particleLifeSpan = 10.0f, float size = 0.5f, Vector4 col = {1, 1, 1, 1})
		: spawnRate(spawnRate),
		spawnWidth(spawnWidth),
		spawnDepth(spawnDepth),
		spawnHeight(spawnHeight),
		position(pos),
		globalAcceleration(globalAcc),
		particleLifeSpan(particleLifeSpan),
		timeSinceLastSpawn(0.0f),
		color(col)
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
		// Crear nuevas partículas basadas en spawn rate
		timeSinceLastSpawn += t;

		while (timeSinceLastSpawn >= (1.0f / spawnRate))
		{
			float randomX = randomFloat(-spawnWidth / 2.0f, spawnWidth / 2.0f);
			float randomY = randomFloat(0.0f, spawnHeight);
			float randomZ = randomFloat(-spawnDepth / 2.0f, spawnDepth / 2.0f);

			Vector3D spawnPos = position + Vector3D(randomX, randomY, randomZ);

			Vector3D initialVelocity = Vector3D(0, 0, 0);

			// Crear nueva partícula (Vector3D Pos, Vector3D Vel, Vector3D Accel, float mass, float lifespam, Vector4 col, float size)
			Particle* newParticle = new Particle(spawnPos, initialVelocity, globalAcceleration, 1.0f, particleLifeSpan, color, 0.2f);
			addParticle(newParticle);

			timeSinceLastSpawn -= (1.0f / spawnRate);
		}

		clearDeadParticles();
	}

	void integrateAll(double t)
	{
		for (Particle* p : particles)
		{
			for (ForceGenerator* fg : forceGenerators)
			{
				fg->updateForce(p, t);
			}
			p->integrate(t);
		}
	}

	void clearDeadParticles()
	{
		for (auto it = particles.begin(); it != particles.end();)
		{
			// Eliminar partículas muertas
			if (!(*it)->isAlive())
			{
				delete* it;
				it = particles.erase(it);
				//it = particles.erase(std::remove(particles.begin(), particles.end(), *it), particles.end());
				//dParticles.push_back(it);
			}
			else {
				++it;
			}
		}
		//for (Particle* it : dParticles)
		//{
		//	particles.erase(std::remove(particles.begin(), particles.end(), it), particles.end());
		//    //delete it;
		//}
		//dParticles.clear();
	}
};

