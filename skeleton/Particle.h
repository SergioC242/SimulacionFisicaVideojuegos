#include "Vector3D.h"
#include "RenderUtils.hpp"
#include <PxPhysicsAPI.h>

#pragma once
class Particle
{
public:
	Particle(Vector3D Pos, Vector3D Vel, Vector3D Accel = (0, 0, 0), float mass = 0.0f, float lifespam = 2.0f);
	~Particle();

	void integrate(float duration);
	void changeAcceleration(Vector3D Acc) { acceleration = Acc; };
	float getCineticEnergy() { return 0.5f * Mass * vel.Modulo() * vel.Modulo(); };

	bool isAlive() const { return LifeSpan > 0.0f; };
	float getMass() const { return Mass; };
	void addForce(const Vector3D& f) {
		force += f;
	}

	Vector3D getVelocity() const { return vel; }
	physx::PxTransform* getPose() const { return pose; }


private:
	Vector3D vel;
	physx::PxTransform* pose = nullptr;
	RenderItem* renderItem = nullptr;
	Vector3D acceleration = Vector3D(0, 0, 0);
	float damping = 0.99;
	float LifeSpan; 
	float Mass; // Not used if not projectile
	Vector3D force;
};

