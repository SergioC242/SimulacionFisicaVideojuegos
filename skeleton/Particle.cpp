#include "Particle.h"

Particle::Particle(Vector3D Pos, Vector3D Vel, Vector3D Accel, float mass) : vel(Vel), Mass(mass), acceleration(Accel)
{
    pose = new physx::PxTransform(physx::PxVec3(Pos.getX(), Pos.getY(), Pos.getZ()));
    renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(0.5f)), pose, Vector4(1, 0, 0, 1));
}

Particle::~Particle()
{
    delete renderItem;
    delete pose;
}


void Particle::integrate(float duration)
{
	//metodo integracion de Euler semi-implicito
    if (duration <= 0.0f) return;

    // Actualizar velocidad con la aceleración (Euler semi-implícito)
    vel += acceleration * duration;
    
    // Aplicar amortiguamiento
    vel *= powf(damping, duration);

    // Actualizar posición con la nueva velocidad
    pose->p.x += vel.getX() * duration;
    pose->p.y += vel.getY() * duration;
    pose->p.z += vel.getZ() * duration;
}
