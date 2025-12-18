#pragma once

#include "ForceGenerator.h"
#include "core.hpp"     

class BuoyancyForceGenerator : public ForceGenerator
{
public:
	BuoyancyForceGenerator( float V, float d, float h = 0.0f)
		: _height(h), _volume(V), _liquid_density(d), _gravity(9.8f),
		_liquid_particle(nullptr)
	{
	}

	inline void setLiquidParticle(Particle* p) { _liquid_particle = p; _height = _liquid_particle->getPos()->p.y; }

	// Actualiza la fuerza de flotación sobre la partícula
	void updateForce(Particle* particle, float duration) override;

	virtual ~BuoyancyForceGenerator() {}

protected:
	float _height;          // Altura total del objeto
	float _volume;          // Volumen sumergible
	float _liquid_density;  // Densidad del líquido
	float _gravity;         // Constante g

	Particle* _liquid_particle;   // Nivel del agua (posición Y)
};
