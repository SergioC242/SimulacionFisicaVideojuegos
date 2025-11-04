#pragma once
#include "ForceGenerator.h"
#include "Vector3D.h"

class GravityForceGenerator : public ForceGenerator
{
private:
    Vector3D gravity; // Aceleración de la gravedad (ej: (0, -9.8, 0))

public:
    GravityForceGenerator(const Vector3D& g) : gravity(g) {}

    void updateForce(Particle* particle, float duration) override
    {
        // No aplica fuerza si la masa es infinita o nula
        if (particle->getMass() <= 0.0f) return;

        // F = m * g
        Vector3D force = gravity * particle->getMass();
        particle->addForce(force);
    }
};
