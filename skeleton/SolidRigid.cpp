#include "SolidRigid.h"

SolidRigid::SolidRigid()
	: gFoundation(nullptr), gPhysics(nullptr), gScene(nullptr), mMaterial(nullptr)
{
}

SolidRigid::~SolidRigid()
{
	for (RenderItem* item : renderItems)
	{
		delete item;
	}

}

bool SolidRigid::initPhysics(PxPhysics* gP, PxScene* gS)
{

	// Physics object
	gPhysics = gP;

	// Scene
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	//sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	gScene = gS;

	// Default material
	mMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	return true;
}

void SolidRigid::stepPhysics(float dt)
{
	for (PxRigidDynamic* actor : dynamicActors)
	{
		// Aplicar fuerzas de los generadores de fuerzas
		for (ForceGenerator* fg : forceGenerators)
		{
			fg->updateForce(actor, dt);
		}
	}
	if (gScene)
	{
		gScene->simulate(dt);
		gScene->fetchResults(true);
	}
}

PxRigidStatic* SolidRigid::createStaticBox(const PxVec3& pos, const PxVec3& halfExtents)
{
	PxRigidStatic* actor = gPhysics->createRigidStatic(PxTransform(pos));

	PxShape* shape = actor->createShape(PxBoxGeometry(halfExtents), *mMaterial);


	renderItems.push_back(new RenderItem(shape, actor, Vector4(0.5f, 0.5f, 0.5f, 1.0f)));
	gScene->addActor(*actor);
	return actor;
}

PxRigidDynamic* SolidRigid::createDynamicBox(const PxVec3& pos, const PxVec3& halfExtents,
	float density,
	const PxVec3& initLinearVel)
{
	PxRigidDynamic* actor = gPhysics->createRigidDynamic(PxTransform(pos));

	PxShape* shape = actor->createShape(PxBoxGeometry(halfExtents), *mMaterial);

	// Masa e inercia automática
	PxRigidBodyExt::updateMassAndInertia(*actor, density);

	// Velocidad inicial
	//actor->setLinearVelocity(initLinearVel);

	//make it not have gravity
	actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	actor->setMassSpaceInertiaTensor(PxVec3(halfExtents * 4));
	renderItems.push_back(new RenderItem(shape, actor, Vector4(0.5f, 0.5f, 0.5f, 1.0f)));
	gScene->addActor(*actor);
	dynamicActors.push_back(actor);
	return actor;
}

PxRigidDynamic* SolidRigid::createDynamicSphere(const PxVec3& pos, float radius, float density, const PxVec3& initLinearVel) {
	PxRigidDynamic* actor = gPhysics->createRigidDynamic(PxTransform(pos));
	PxShape* shape = actor->createShape(PxSphereGeometry(radius), *mMaterial);
	// Masa e inercia automática
	PxRigidBodyExt::updateMassAndInertia(*actor, density);

	actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	actor->setLinearVelocity(initLinearVel);
	renderItems.push_back(new RenderItem(shape, actor, Vector4(0.5f, 0.5f, 0.5f, 1.0f)));
	gScene->addActor(*actor);
	dynamicActors.push_back(actor);
	return actor;
}
