#include "Vector3D.h"
#include "RenderUtils.hpp"
#include <PxPhysicsAPI.h>

#pragma once
class Particle
{
public:
	Particle(Vector3D Pos, Vector3D Vel, Vector3D Accel = (0, 0, 0), float mass = 0.0f);
	~Particle();

	void integrate(float duration);
	void changeAcceleration(Vector3D Acc) { acceleration = Acc; };
	float getCineticEnergy() { return 0.5f * Mass * vel.Modulo() * vel.Modulo(); };


private:
	Vector3D vel;
	physx::PxTransform* pose;
	RenderItem* renderItem;
	Vector3D acceleration = Vector3D(0, 0, 0);
	float damping = 0.99;
	float Mass; // Not used if not projectile
};

