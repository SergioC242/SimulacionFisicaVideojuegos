#include "ExplosionGenerator.h"


ExplosionGenerator::ExplosionGenerator(const Vector3D& pos, float range, float fuerza, float time, float timAct, bool active) :
	mPos(pos), range(range), fuerza(fuerza), time(0), timAct(timAct), active(active)
{
}

Vector3D ExplosionGenerator::getForce(Particle* aux)
{
	if (!aux || aux->getMass() <= 0.0f) return Vector3D(0, 0, 0);

	// Obtener posición de la partícula desde PxTransform
	physx::PxVec3 p = aux->getPos()->p;
	Vector3D pos(p.x, p.y, p.z);

	Vector3D vector = pos - mPos;
	float distancia = vector.Modulo();
	
	if (range >= distancia)
	{


		// Comprobaciones de rango y activación
		if (!active) return Vector3D(0, 0, 0);
		if (distancia <= 0.0f) return Vector3D(0, 0, 0); // evitar división por cero
		if (distancia > range) return Vector3D(0, 0, 0);

		// Intensidad que decae con el cuadrado de la distancia
		float powerF = fuerza / (distancia * distancia);

		// Factor temporal (seguí la lógica original usando `time` y `timAct`)
		float dT = ((time - timAct) / time);
		float timeF = expf(-dT); // e^{-dT}

		// Dirección proporcional al vector desde la explosión hacia la partícula
		float rX = powerF * vector.getX() * timeF;
		float rY = powerF * vector.getY() * timeF;
		float rZ = powerF * vector.getZ() * timeF;

		return Vector3D(rX, rY, rZ);
	}
	else
	{
		return Vector3D(0, 0, 0);
	}
}

void ExplosionGenerator::updateForce(Particle* particle, float d/*duration*/)
{
	time += d;
	if(gTime)
	{
		gTime = false;
		timAct = time;
	}
	if(time/10 >= activeTime + timAct/10)
	{
		active = false;
	}
	// Aplicar la fuerza calculada directamente a la partícula
	if (!particle) return;
	Vector3D f = getForce(particle);
	particle->addForce(f);
}