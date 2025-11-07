#pragma once
#include "ForceGenerator.h"
#include "Vector3D.h"

class WindForceGenerator : public ForceGenerator
{
private:
    Vector3D windVelocity;  
    float k1;               
    float k2;               
    Vector3D areaMin;       
    Vector3D areaMax;

public:
    WindForceGenerator(const Vector3D& windVel, float k1 = 0.1f, float k2 = 0.0f,
        Vector3D areaMin = Vector3D(-100, -100, -100),
        Vector3D areaMax = Vector3D(100, 100, 100))
        : windVelocity(windVel), k1(k1), k2(k2), areaMin(areaMin), areaMax(areaMax) {
    }

    void updateForce(Particle* particle, float duration) override
    {
        if (particle->getMass() <= 0.0f) return;

        physx::PxVec3 pos = particle->getPose()->p;
		//Si no esta dentro del area
        if (pos.x < areaMin.getX() || pos.x > areaMax.getX() ||
            pos.y < areaMin.getY() || pos.y > areaMax.getY() ||
            pos.z < areaMin.getZ() || pos.z > areaMax.getZ())
        {
            return;
        }

        Vector3D relativeVel = windVelocity - particle->getVelocity();
        Vector3D force = relativeVel * k1;

        if (k2 != 0.0f)
        {
            float speed = relativeVel.Modulo();
            force += relativeVel * (k2 * speed);
        }

        // aplicar la fuerza a la partícula
        particle->addForce(force);
    }

    void setWindVelocity(const Vector3D& windVel) {
        windVelocity = windVel;
	}
};
