#include "Vector3D.h"
#include "RenderUtils.hpp"
#include <PxPhysicsAPI.h>

#pragma once
class Particle
{
public:
	Particle(Vector3D Pos, Vector3D Vel);
	~Particle();

	void integrate(float duration);
	void changeAcceleration(Vector3D Acc) { acceleration = Acc; };

private:
	Vector3D vel;
	physx::PxTransform* pose;
	RenderItem* renderItem;
	Vector3D acceleration = Vector3D(0, 0, 0);
	float damping = 0.99;
};

