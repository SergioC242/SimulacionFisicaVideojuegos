#include "Vector3D.h"
#include "RenderUtils.hpp"
#include <PxPhysicsAPI.h>

class ForceGenerator;

#pragma once
class Particle
{
public:
	Particle(Vector3D Pos, Vector3D Vel, Vector3D Accel = (0, 0, 0), float mass = 0.0f, float lifespam = 2.0f, Vector4 col = { 0, 0, 0.5, 1 }, float size = 0.5f);
	~Particle();

	void integrate(float duration);
	void changeAcceleration(Vector3D Acc) { acceleration = Acc; };
	float getCineticEnergy() { return 0.5f * Mass * vel.Modulo() * vel.Modulo(); };

	bool isAlive() const { return LifeSpan > 0.0f; };
	float getMass() const { return Mass; };
	void setMass(float m) { Mass = m; };
	void addForce(const Vector3D& f) {
		force += f;
	}

	void addForceGenerator(ForceGenerator* fg) {
		forceGenerators.push_back(fg);
	}

	void setVelocity(const Vector3D& v) { vel = v; } //solo para que el barco rote la velocidad segun su rumbo NO USAR PARA NADA MAS

	Vector3D getVelocity() const { return vel; }
	physx::PxTransform* getPos() const { return pos; }

	float getHeight() const { return rad * 2; }


private:
	Vector3D vel;
	physx::PxTransform* pos = nullptr;
	RenderItem* renderItem = nullptr;
	Vector3D acceleration = Vector3D(0, 0, 0);
	float damping = 0.99;
	float LifeSpan;
	float Mass; // Not used if not projectile
	Vector3D force;

	float rad = 0.5f;

	//vector de forces
	std::vector<ForceGenerator*> forceGenerators;
};

