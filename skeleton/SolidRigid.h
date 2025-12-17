#pragma once

#include <PxPhysicsAPI.h>
#include "RenderUtils.hpp"
#include "ForceGenerator.h"

using namespace physx;

//PxDefaultAllocator	gAllocator;


class SolidRigid
{

public:
	SolidRigid();
	~SolidRigid();

	// Inicialización y limpieza de PhysX
	bool initPhysics(PxPhysics* gP, PxScene* gS);
	void stepPhysics(float dt);

	// Manejo de colisiones
	void onCollision(physx::PxActor* actor1, physx::PxActor* actor2)
	{
		PX_UNUSED(actor1);
		PX_UNUSED(actor2);
	}
	void addForceGenerator(ForceGenerator* fg) {
		forceGenerators.push_back(fg);
	}

	// Creación de actores
	PxRigidStatic* createStaticBox(const PxVec3& pos, const PxVec3& halfExtents);
	PxRigidDynamic* createDynamicBox(const PxVec3& pos, const PxVec3& halfExtents,
		float density,
		const PxVec3& initLinearVel = PxVec3(0, 0, 0));

private:
	PxFoundation* gFoundation;
	PxPhysics* gPhysics;
	PxScene* gScene;
	PxDefaultAllocator    gAllocator;
	PxDefaultErrorCallback mErrorCallback;
	PxMaterial* mMaterial;
	std::vector<RenderItem*> renderItems;
	std::vector<ForceGenerator*> forceGenerators;
	std::vector<PxRigidDynamic*> dynamicActors;
};

