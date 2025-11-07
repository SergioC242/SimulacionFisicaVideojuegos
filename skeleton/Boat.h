#pragma once
#include "Particle.h"
#include "WindForceGenerator.h"
#include "RenderUtils.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

class Boat : public Particle
{
private:
    Vector3D windVelocity;          // viento global
    float headingAngle;             // rumbo del barco (radianes)
    float turnRate;                 // velocidad angular (radianes/seg)
    float palaAngle;              // timon (-1 izquierda, 1 derecha)
    float sailAngle;                // angulo de vela respecto al barco (grados)
    float sailChangeRate;           // velocidad de cazar/largar
    float maxSideSpeedMultiplier;   // multiplicador velocidad al traves
    const float sailEffectFactor = 0.5f;
	const float criticalAngleCeñida = 20.0f; // angulo critico para ceñida

public:
    Boat(Vector3D position, const Vector3D& windVel, float headingDeg = 0.0f)
        : Particle(position, Vector3D(0, 0, 0), Vector3D(0, 0, 0), 50.0f),
        windVelocity(windVel),
        headingAngle(headingDeg* (3.14159f / 180.0f)),
        turnRate(1.0f),
        palaAngle(0.0f),
        sailAngle(45.0f),       // vela inicial a 45 
        sailChangeRate(30.0f),  // grados por segundo
        maxSideSpeedMultiplier(1.2f)
    {
        // Cuerpo del barco (bloque azul)
        physx::PxShape* hullShape = CreateShape(physx::PxBoxGeometry(2.0f, 0.2f, 1.0f));
        Vector4 colorHull = { 0.2f, 0.3f, 1.0f, 1.0f };
        new RenderItem(hullShape, getPose(), colorHull);
    }

    void update(float t)
    {
        // Actualiza la rotacion del barco segun timon
        headingAngle += palaAngle * turnRate * t;

		//saca el angulo en celsioes entre 0 y 360
        if (headingAngle < 0.0f)
            headingAngle += 2.0f * 3.14159f;
        else if (headingAngle >= 2.0f * 3.14159f)
			headingAngle -= 2.0f * 3.14159f;
		std::cout << "Boat Heading: " << headingAngle * (180.0f / 3.14159f) << " degrees" << std::endl;

        // Calcula las fuerzas del viento y aplica
        applyWindPhysics(t);

        // Integra movimiento
        integrate(t);

        // Actualiza orientacion visual del barco
        getPose()->q = physx::PxQuat(headingAngle, physx::PxVec3(0, 1, 0));

        // --- Ajuste de la velocidad al girar (mantiene velocidad pero reorienta suavemente) ---

        Vector3D currentVel = getVelocity();
        float speed = currentVel.Modulo();

        // Solo si el barco tiene velocidad apreciable
        if (speed > 0.01f)
        {
            // Dirección actual de la velocidad
            Vector3D velDir = currentVel.Normalize();

            // Nueva dirección según el rumbo del barco
            Vector3D forwardDir(std::cos(headingAngle), 0.0f, std::sin(headingAngle));
            float alignmentFactor = t * 1.5f;
            if (alignmentFactor > 1.0f) alignmentFactor = 1.0f;
            if (alignmentFactor < 0.0f) alignmentFactor = 0.0f;

            // Calcula nueva dirección suavemente girada hacia la proa
            Vector3D newDir = (velDir * (1.0f - alignmentFactor) + forwardDir * alignmentFactor).Normalize();

            // Mantiene el mismo módulo (energía)
            setVelocity(newDir * speed);
        }

		stopTurn();

        //cout angulo de vela
		std::cout << "Sail Angle: " << sailAngle << " degrees" << std::endl;
    }

    // Control del timon
    void turnLeft() { palaAngle = 1.0f; }   // vira a la izquierda
    void turnRight() { palaAngle = -1.0f;}  // vira a la derecha
    void stopTurn() { palaAngle = 0.0f;}   // cuando no se pulsa nada

    // Control de la vela (angulo 5 –90 )
    void cazarVela(float t)   // reduce angulo
    {
        sailAngle -= sailChangeRate * t;
        if (sailAngle < 5.0f) sailAngle = 5.0f;
    }

    void largarVela(float t) // aumenta angulo
    {
        sailAngle += sailChangeRate * t;
        if (sailAngle > 90.0f) sailAngle = 90.0f;
    }

    float getSailAngle() const { return sailAngle; }

    void setWind(const Vector3D& newWind) { windVelocity = newWind; }

private:
    void applyWindPhysics(float t)
    {
        if (getMass() <= 0.0f) return;

        Vector3D boatDir(std::cos(headingAngle), 0.0f, std::sin(headingAngle));
        Vector3D boatVel = getVelocity();
        Vector3D relWind = windVelocity - boatVel;

        float boatWindAngle = boatDir.Normalize().Escalar(relWind.Normalize());
        float angleDeg = std::acos(boatWindAngle) * (180.0f / 3.14159f);

		std::cout << "Angle between boat and wind: " << angleDeg << " degrees" << std::endl;

        // ---- Eficiencia segun angulo barco viento ----
        float efficiency = 0.0f;
        float windSpeed = windVelocity.Modulo();

		// ---- Contra el viento ---- (no navega)
        if (angleDeg <= criticalAngleCeñida)
        {
            efficiency = 0.0f;  // sin fuerza de vela
        }
        // ---- Ceñida ----
        if (angleDeg > criticalAngleCeñida &&  angleDeg < 70.0f)
        {
            float sailEffect = max(0.0f, 1.0f - (sailAngle - 5.0f) / 35.0f);
            efficiency = 0.6f * sailEffect;

        }
        // ---- Al traves (≈90 ) ----
        else if (angleDeg >= 70.0f && angleDeg <= 110.0f)
        {
            // Maxima eficiencia base = 60 , multiplicada por efecto del angulo vela
            float sailEffect = 1.0f - std::abs(sailAngle - 45.0f) / 45.0f; // mejor rendimiento con 45 
            efficiency = (0.6f + 0.2f) * max(0.0f, sailEffect); // 20  extra por travesia
        }
        // ---- Popa (viento detras) ----
        else if (angleDeg > 160.0f)
        {
            // Cuanto mas cerca del viento directo (180 ), mas rendimiento
            float rel = min(1.0f, (angleDeg - 160.0f) / 20.0f); // 160 →0 , 180 →100 
            efficiency = 0.6f * rel * (1.0f - (sailAngle - 90.0f) / 90.0f); // vela abierta rinde mas
            if (efficiency < 0.0f) efficiency = 0.0f;
        }
        // ---- Intermedios ----
        else
        {
            // Interpola eficiencia entre 0 y 0.6 dependiendo de angulo
            efficiency = 0.6f * max(0.0f, 1.0f - std::abs(angleDeg - 90.0f) / 90.0f);
        }

        //if (efficiency > 0.0f)
        //{
        //    Vector3D forceDir = boatDir;
        //    Vector3D force = forceDir * (windSpeed * efficiency);
        //  addForce(force);
        //}

        // --- Fuerza de la vela ---

        // Magnitud proporcional a la velocidad del viento y a la eficiencia de la vela
        windSpeed = relWind.Modulo();
        float sailForceMagnitude = windSpeed * efficiency;

        // Dirección del empuje: el rumbo del barco (proa)
        //Vector3D sailForceDir = boatDir.Normalize();
        // Fuerza final aplicada al barco
        //Vector3D sailForce = sailForceDir * sailForceMagnitude * sailEffectFactor;

        Vector3D localForward(0, 0, 1);

        // Transformarla según la rotación actual del barco
        physx::PxQuat rotation = getPose()->q; // orientación real del cuerpo
        physx::PxVec3 dirGlobal = rotation.rotate(physx::PxVec3(localForward.getX(), localForward.getY(), localForward.getZ()));

        Vector3D forwardDir(dirGlobal.x, dirGlobal.y, dirGlobal.z);
        Vector3D sailForce = localForward * windSpeed * efficiency * sailEffectFactor * 100;
        // Aplicamos la fuerza a la partícula del barco
        addForce(sailForce);

        //Fuerza de deriva 
        Vector3D sideDir(-boatDir.getZ(), 0, boatDir.getX()); // perpendicular en el plano XZ
        float sideForceMag = windSpeed * (1.0f - efficiency) * 0.2f;
        Vector3D sideForce = sideDir * sideForceMag;
        //addForce(sideForce);



		// ---- Simula la orza ----
        // Dirección perpendicular al rumbo del barco
        Vector3D rightDir(-boatDir.getZ(), 0.0f, boatDir.getX());


        float lateralSpeed = boatVel.Escalar(rightDir);

        float horizontalSpeed = sqrt(boatVel.getX() * boatVel.getX() + boatVel.getZ() * boatVel.getZ());
        // Eficiencia dinámica de la orza: de 50% a 90% según velocidad horizontal
        float keelEfficiency = 0.5f + min(0.4f, 0.4f * (horizontalSpeed / 10.0f));
        // (a 10 m/s o más, llega al 90%)

        // Aplicamos la fuerza opuesta al movimiento lateral
        Vector3D keelForce = rightDir * (-lateralSpeed * getMass() * keelEfficiency);

        // Aplica solo si es significativo
        if (std::abs(lateralSpeed) > 0.01f);
            //addForce(keelForce);

    }
};
