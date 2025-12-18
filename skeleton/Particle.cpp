

#include "Particle.h"

#include "ForceGenerator.h"

Particle::Particle(Vector3D Pos, Vector3D Vel, Vector3D Accel, float mass, float lifespam, Vector4 col, float size) :
	vel(Vel), Mass(mass), acceleration(Accel), LifeSpan(lifespam), rad(size)
{
	pos = new physx::PxTransform(physx::PxVec3(Pos.getX(), Pos.getY(), Pos.getZ()));
	renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(rad)), pos, col);

	volume = (4.0f / 3.0f) * 3.1416f * rad * rad * rad;
}

Particle::~Particle()
{
	//if (renderItem) {
	//	//delete renderItem;
	//	//renderItem->release();
	//	//renderItem = nullptr;
	//}
	if (pos) {
		delete pos;
		pos = nullptr;
	}

}


void Particle::integrate(float duration)
{
	// Actualizar el tiempo de vida
	if(LifeSpan != -1) LifeSpan -= duration;
	//metodo integracion de Euler semi-implicito
	if (duration <= 0.0f) return;
	for (ForceGenerator* fg : forceGenerators)
	{
		fg->updateForce(this, duration);
	}
	Vector3D totalAcc = acceleration + (force * (1.0f / Mass));

	// Actualizar velocidad con la aceleración (Euler semi-implícito)
	vel += totalAcc * duration;

	// Aplicar amortiguamiento
	vel *= powf(damping, duration);

	// Actualizar posición con la nueva velocidad
	pos->p.x += vel.getX() * duration;
	pos->p.y += vel.getY() * duration;
	pos->p.z += vel.getZ() * duration;

	force = Vector3D(0, 0, 0);
}
