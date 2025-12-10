#include "BuoyancyForceGenerator.h"

void BuoyancyForceGenerator::updateForce(Particle* p, float duration)
{
    if (p->getMass() <= 0.0f) return;
    if (_liquid_particle == nullptr) return; 
    
    // Alturas
    float h = p->getPos()->p.y;                 // Altura del objeto
    float h0 = _liquid_particle->getPos()->p.y; // Nivel del agua

    Vector3 f(0, 0, 0);
    float immersed = 0.0f;

    // 1) Está completamente fuera del agua
    _height = p->getHeight();
    if (h - h0 > _height * 0.5f) {
        immersed = 0.0f;
    }
    // 2) Está totalmente sumergido
    else if (h0 - h > _height * 0.5f) {
        immersed = 1.0f;
    }
    // 3) Parcialmente sumergido
    else {
        immersed = (h0 - h) / _height + 0.5f;
    }

    // Fuerza de flotación
    f.y = _liquid_density * _volume * immersed * _gravity;

    p->addForce({f.x, f.y, f.z});
}
