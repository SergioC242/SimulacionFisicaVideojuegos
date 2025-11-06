#pragma once
#include "Particle.h"
#include "WindForceGenerator.h"
#include "RenderUtils.hpp"
#include <cmath>
#include <iostream>
#include <algorithm> 
class Boat2 : public Particle
{
private:
    Vector3D windVelocity;          // viento global
    float headingAngle;             // rumbo del barco (radianes)
    float turnRate;                 // velocidad angular (radianes/seg)
    float palaAngle;                // timon (-1 izquierda, 1 derecha)
    float sailAngle;                // angulo de vela respecto al barco (grados)
    float sailChangeRate;           // velocidad de cazar/largar
    float maxSideSpeedMultiplier;   // multiplicador velocidad al traves
    const float sailEffectFactor = 0.5f;
    const float criticalAngleCeñida = 20.0f; 

public:
    Boat2(Vector3D position, const Vector3D& windVel, float headingDeg = 0.0f)
        : Particle(position, Vector3D(0, 0, 0), Vector3D(0, 0, 0), 50.0f),
        windVelocity(windVel),
        headingAngle(headingDeg* (3.14159f / 180.0f)),
        turnRate(5.0f),
        palaAngle(0.0f),
        sailAngle(45.0f),       
        sailChangeRate(30.0f),  
        maxSideSpeedMultiplier(1.2f)
    {
        
        physx::PxShape* hullShape = CreateShape(physx::PxBoxGeometry(2.0f, 0.2f, 1.0f));
        Vector4 colorHull = { 0.2f, 0.3f, 1.0f, 1.0f };
        new RenderItem(hullShape, getPose(), colorHull);
    }


    void turnLeft() { palaAngle = 1.0f; }
    void turnRight() { palaAngle = -1.0f; } 
    void stopTurn() { palaAngle = 0.0f; }   

    // Control vela
    void cazarVela(float t)  
    {
        sailAngle -= sailChangeRate * t;
        if (sailAngle < 5.0f) sailAngle = 5.0f;
    }

    void largarVela(float t)
    {
        sailAngle += sailChangeRate * t;
        if (sailAngle > 90.0f) sailAngle = 90.0f;
    }

    void MoveForward()
    {
        Vector3D localForward(-1, 0, 0);

        // Transformarla según la rotación actual del barco
        physx::PxQuat rotation = getPose()->q; // orientación real del cuerpo
        physx::PxVec3 dirGlobal = rotation.rotate(physx::PxVec3(localForward.getX(), localForward.getY(), localForward.getZ()));

        Vector3D forwardDir(dirGlobal.x, dirGlobal.y, dirGlobal.z);

        // Aplicar la fuerza en la dirección que apunta la proa
        Vector3D forwardForce = forwardDir * 1000.0f;
        addForce(forwardForce);
    }
    void update(float t)
    {
        headingAngle += palaAngle * turnRate * t;
        if (headingAngle < 0.0f)
            headingAngle += 2.0f * 3.14159f;
        else if (headingAngle >= 2.0f * 3.14159f)
            headingAngle -= 2.0f * 3.14159f;
        integrate(t);
        getPose()->q = physx::PxQuat(headingAngle, physx::PxVec3(0, 1, 0));
        // Ajuste de la velocidad al girar
        Vector3D currentVel = getVelocity();
        float speed = currentVel.Modulo();
        //if (speed > 0.0f)
        //{
        //    Vector3D velDir = currentVel.Normalize();
        //    Vector3D forwardDir(std::cos(headingAngle), 0.0f, std::sin(headingAngle));
        //    float alignmentFactor = t * 1.5f;
        //    if (alignmentFactor > 1.0f) alignmentFactor = 1.0f;
        //    if (alignmentFactor < 0.0f) alignmentFactor = 0.0f;
        //    Vector3D newDir = (velDir * (1.0f - alignmentFactor) + forwardDir * alignmentFactor).Normalize();
        //    setVelocity(newDir * speed);
        //}
        stopTurn();
	}
};

