#pragma once

#include "ForceGenerator.h"

class SpringForceGenerator : public ForceGenerator
{


private:
protected:
	double _k;                    // Elastic Coeff.
	double _resting_length;
	Particle* p2;

public:
	SpringForceGenerator(Particle* P2, const double SS, double restingLength)
		: _k(SS), p2(P2), _resting_length(restingLength) {
	}

	inline void setK(double k) { _k = k; }

	void updateForce(Particle* particle, float duration) override
	{
		if (particle->getMass() <= 0.0f) return;

		//physx::PxVec3 pos = particle->getPose()->p;
		//Si no existe la segunda particula
		if (p2 == nullptr)
		{
			return;
		}

		Vector3 relative_pos_vector = p2->getPose()->p - particle->getPose()->p;
		Vector3 force;

		// normalize: Normalize the relative_pos_vector and returns its length.
		const float length = relative_pos_vector.normalize();
		const float delta_x = length - _resting_length;

		force = relative_pos_vector * delta_x * _k;

		particle->addForce({force.x, force.y, force.z});
	}

};

