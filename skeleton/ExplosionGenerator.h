#pragma once

#include "ForceGenerator.h"
#include "Vector3D.h"
#include "Particle.h"

class ExplosionGenerator : public ForceGenerator
{
private:
	Vector3D mPos;
	float range;
	float fuerza;
	float time;    // tiempo total de la explosión (segundos)
	float timAct;  // tiempo actual o tiempo de activación
	bool active;

public:
	ExplosionGenerator(const Vector3D& pos = Vector3D(0, 0, 0),
		float range = 10.0f,
		float fuerza = 100.0f,
		float time = 1.0f,
		float timAct = 0.0f,
		bool active = false);

	// Devuelve la fuerza (Vector3D) que la explosión aplica a la partícula
	Vector3D getForce(Particle* aux);

	// Implementación del contrato ForceGenerator
	void updateForce(Particle* particle, float duration) override;

	// Helpers
	void setPosition(const Vector3D& pos) { mPos = pos; }
	void setActive(bool a) { active = a; }
	void trigger(float currentTime) { timAct = currentTime; active = true; }
};