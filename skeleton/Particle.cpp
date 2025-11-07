#include "Particle.h"

Particle::Particle(Vector3D Pos, Vector3D Vel, Vector3D Accel, float mass, float lifespam) : 
    vel(Vel), Mass(mass), acceleration(Accel), LifeSpan(lifespam)
{
    pose = new physx::PxTransform(physx::PxVec3(Pos.getX(), Pos.getY(), Pos.getZ()));
    renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(0.5f)), pose, Vector4(0, 0, 0.5, 1));
}

Particle::~Particle()
{
    if (renderItem) {
        delete renderItem;
        renderItem = nullptr;
    }
    if (pose) {
        delete pose;
        pose = nullptr;
    }
    
}


void Particle::integrate(float duration)
{
	// Actualizar el tiempo de vida
    LifeSpan -= duration;
	//metodo integracion de Euler semi-implicito
    if (duration <= 0.0f) return;

    Vector3D totalAcc = acceleration + (force * (1.0f / Mass));

    // Actualizar velocidad con la aceleración (Euler semi-implícito)
    vel += totalAcc * duration;
    
    // Aplicar amortiguamiento
    vel *= powf(damping, duration);

    // Actualizar posición con la nueva velocidad
    pose->p.x += vel.getX() * duration;
    pose->p.y += vel.getY() * duration;
    pose->p.z += vel.getZ() * duration;

    force = Vector3D(0, 0, 0);
}
