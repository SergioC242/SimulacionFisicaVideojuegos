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
        turnRate(2.0f),
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

		WindMagic(t);
        integrate(t);
        applyKeelEffect(t);
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

    void applyKeelEffect(float t)
    {
        Vector3D currentVel = getVelocity();
        float speed = currentVel.Modulo();

        if (speed < 0.01f) return;

        // Dirección hacia adelante del barco (proa) usando la rotación real
        Vector3D localForward(-1, 0, 0);
        physx::PxQuat rotation = getPose()->q;
        physx::PxVec3 dirGlobal = rotation.rotate(physx::PxVec3(localForward.getX(), localForward.getY(), localForward.getZ()));
        Vector3D forwardDir(dirGlobal.x, dirGlobal.y, dirGlobal.z);

        // Descomponer velocidad en componentes paralela y perpendicular
        float parallelComponent = currentVel.Escalar(forwardDir);
        Vector3D parallelVel = forwardDir * parallelComponent;
        Vector3D perpendicularVel = currentVel - parallelVel;

        // Aplicar resistencia solo a la componente perpendicular (orza)
        // Coeficiente ajustado para que tarde 3-5 segundos en frenar el movimiento lateral
        float keelDragCoefficient = 0.8f * getMass();
        Vector3D keelDragForce = perpendicularVel * (-keelDragCoefficient);
        addForce(keelDragForce);
    }

    void WindMagic(float t) {
        if (getMass() <= 0.0f) return;

        Vector3D boatDir(std::cos(headingAngle), 0.0f, std::sin(headingAngle));
        Vector3D boatVel = getVelocity();
        Vector3D relWind = windVelocity - boatVel;

        float relWindMag = relWind.Modulo();
        if (relWindMag < 0.01f) return;

        float absWindMag = windVelocity.Modulo();
        float boatSpeed = boatVel.Modulo();

        float speedRatio = (absWindMag > 0.01f) ? (boatSpeed / absWindMag) : 0.0f;
        float speedFactor = max(0.0f, 1.0f - speedRatio);
        // Puedes ajustar la curva con un exponente para controlar la caída
        speedFactor = std::pow(speedFactor, 0.8f);

        float boatWindAngle = boatDir.Normalize().Escalar(relWind.Normalize());
        float angleDeg = std::acos(boatWindAngle) * (180.0f / 3.14159f);


        float efficiency = 0.0f;

        // No sailing possible directly into the wind
        if (angleDeg <= 20.0f)
        {
            efficiency = 0.0f;
        }
        else
        {
            // Interpolación del ángulo óptimo de vela
            // 20° -> 5° (vela muy cazada, ceñida)
            // 180° -> 90° (vela totalmente largada, viento en popa)
            float normalizedAngle = (angleDeg - 20.0f) / (180.0f - 20.0f);  // 0 a 1
            float idealSailAngle = 5.0f + normalizedAngle * (90.0f - 5.0f);

            // Eficiencia según diferencia con ángulo ideal
            float sailDiff = fabs(sailAngle - idealSailAngle);
            float sailEffect = max(0.0f, 1.0f - sailDiff / 45.0f);

            // Eficiencia del viento: curva realista de navegación a vela
            // Ceñida (20-45°): eficiencia moderada ~0.4-0.6
            // Través (80-100°): máxima eficiencia ~1.0
            // Largo/Popa (140-180°): eficiencia alta ~0.7-0.8
            float windEffect;
            if (normalizedAngle < 0.3f) // Ceñida (20-68°)
            {
                windEffect = 0.4f + normalizedAngle * 2.0f; // 0.4 a 1.0
            }
            else if (normalizedAngle < 0.6f) // Través (68-116°)
            {
                windEffect = 1.0f; // Máxima eficiencia
            }
            else // Largo y popa (116-180°)
            {
                float t = (normalizedAngle - 0.6f) / 0.4f;
                windEffect = 1.0f - 0.2f * t; // 1.0 a 0.8
            }

            // Eficiencia combinada
            efficiency = windEffect * sailEffect * speedFactor;

            std::cout << "Angle: " << angleDeg << "° | Ideal Sail: " << idealSailAngle
                << "° | Current Sail: " << sailAngle << "° | Wind: " << windEffect
                << " | Sail: " << sailEffect << " | Efficiency: " << efficiency << std::endl;


            if (efficiency > 0.0f)
            {
                Vector3D localForward(-1, 0, 0);

                // Transformarla según la rotación actual del barco
                physx::PxQuat rotation = getPose()->q; // orientación real del cuerpo
                physx::PxVec3 dirGlobal = rotation.rotate(physx::PxVec3(localForward.getX(), localForward.getY(), localForward.getZ()));

                Vector3D forwardDir(dirGlobal.x, dirGlobal.y, dirGlobal.z);
				Vector3D relWindDir = relWind.Normalize();


                // Fuerza proporcional a la eficiencia y al viento relativo
                //float forceMagnitude = efficiency * relWindMag * 10.0f; 
                float   forceMagnitude = efficiency * relWindMag * relWindMag * 2.0f;
                Vector3D windForce =  forwardDir * forceMagnitude;
                addForce(windForce);

                Vector3D localSide(0, 0, 1); // lado derecho del barco en coordenadas locales
                physx::PxVec3 sideGlobal = rotation.rotate(physx::PxVec3(localSide.getX(), localSide.getY(), localSide.getZ()));
                Vector3D sideDir(sideGlobal.x, sideGlobal.y, sideGlobal.z);

                // La componente lateral del viento relativo
                float sideWindComponent = relWindDir.Escalar(sideDir);

                // Fuerza lateral proporcional a la componente lateral del viento y a la ineficiencia
                float sideForceMag = sideWindComponent * relWindMag * (1.0f - efficiency) * 0.3f * 10.0f;
                Vector3D sideForce = sideDir * sideForceMag;
                addForce(sideForce);
            }
        }
    }
};

