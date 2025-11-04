#pragma once
#include "Particle.h"

// Clase abstracta para generadores de fuerza
class ForceGenerator
{
public:
    virtual ~ForceGenerator() {}

    // Método virtual puro: cada generador debe implementar cómo aplica su fuerza
    virtual void updateForce(Particle* particle, float duration) = 0;
};
