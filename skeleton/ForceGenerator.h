#pragma once
#include "Particle.h"
#include <PxPhysicsAPI.h>
#include <iostream>

// Clase abstracta para generadores de fuerza
class ForceGenerator
{
public:
    virtual ~ForceGenerator() {}

    // Método virtual puro: cada generador debe implementar cómo aplica su fuerza
    virtual void updateForce(Particle* particle, float duration) = 0;
	// Aviso De que la fuerza no tiene implementación para cuerpos rígidos
    virtual void updateForce(physx::PxRigidDynamic* rigid, float duration) {
		std::cout << "No implementado para cuerpos rígidos" << std::endl;
    }
	// Aviso para cuerpos estáticos ya que no se les puede aplicar fuerza
    virtual void updateForce(physx::PxRigidStatic* rigid, float duration) {
        std::cout << "No es DINAMICO, es estatico" << std::endl;
    }
};
