#include <ctype.h>

#include <PxPhysicsAPI.h>

#include <vector>

#include "core.hpp"
#include "RenderUtils.hpp"
#include "callbacks.hpp"

#include <iostream>

#include "Vector3D.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "GravityForceGenerator.h"
#include "WindForceGenerator.h"


std::string display_text = "IS DEATH THE MEANING OF LIFE? NO ITS DELTARUNE CHAPTER 8";


using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics	= NULL;


PxMaterial*				gMaterial	= NULL;

PxPvd*                  gPvd        = NULL;

PxDefaultCpuDispatcher*	gDispatcher = NULL;
PxScene*				gScene      = NULL;
ContactReportCallback gContactReportCallback;

Particle* myParticle = nullptr;
//<Particle*> canonBall = nullptr;
//vector de punteros particle
std::vector<Particle*> canonballs;

//sistema de ñparticulas
ParticleSystem* Ps;


// Initialize physics engine
void initPhysics(bool interactive)
{
	PX_UNUSED(interactive);

	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true,gPvd);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	// For Solid Rigids +++++++++++++++++++++++++++++++++++++
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = &gContactReportCallback;

	Vector3D ballPos = Vector3D(10, 10, 10);

	PxShape* a = CreateShape(physx::PxSphereGeometry(1));
	PxTransform* b = new PxTransform( 0, 0 , 0 );
	Vector4 c = {1, 1, 1, 1};
	RenderItem* Sphere = new RenderItem(a, b, c);

	PxTransform* b2 = new PxTransform(ballPos.getX(), 0, 0);
	c = { 1, 0, 0, 1 };
	RenderItem* Sphere2 = new RenderItem(a, b2, c);

	PxTransform* b3 = new PxTransform(0, ballPos.getY(), 0);
	c = { 0, 1, 0, 1 };
	RenderItem* Sphere3 = new RenderItem(a, b3, c);

	PxTransform* b4 = new PxTransform(0, 0, ballPos.getZ());
	c = { 0, 0, 1, 1 };
	RenderItem* Sphere4 = new RenderItem(a, b4, c);

	Vector3D particlePos = Vector3D(5, 20, 5);
	Vector3D particleVel = Vector3D(1, 0, 0); // Inicialmente en reposo

	myParticle = new Particle(particlePos, particleVel);

	particleVel = Vector3D(1, 0, 0);
	ballPos = Vector3D(10, 50, 10);
	
	Ps = new ParticleSystem(5.0f, 2.0f, Vector3D(0, 0, 0), 10.0f);
	GravityForceGenerator* gravity1 = new GravityForceGenerator(Vector3D(0, -9.8f, 0));
	//Ps->addForceGenerator(gravity1);
	
	WindForceGenerator* wind1 = new WindForceGenerator(Vector3D(50.0f, 0.0f, 0.0f));
	Ps->addForceGenerator(wind1);

	gScene = gPhysics->createScene(sceneDesc);
	}


// Function to configure what happens in each step of physics
// interactive: true if the game is rendering, false if it offline
// t: time passed since last call in milliseconds
void stepPhysics(bool interactive, double t)
{
	PX_UNUSED(interactive);

	gScene->simulate(t);
	gScene->fetchResults(true);

	myParticle->integrate(t);
	Ps->updateAll(t);
	Ps->integrateAll(t);

	for (auto canonBall : canonballs)
	canonBall->integrate(t);
}

// Function to clean data
// Add custom code to the begining of the function
void cleanupPhysics(bool interactive)
{
	PX_UNUSED(interactive);

	// Rigid Body ++++++++++++++++++++++++++++++++++++++++++
	gScene->release();
	gDispatcher->release();
	// -----------------------------------------------------
	gPhysics->release();	
	PxPvdTransport* transport = gPvd->getTransport();
	gPvd->release();
	transport->release();
	
	for(Particle* canonBall : canonballs)
	{
		delete canonBall;
	}

	gFoundation->release();
	}

// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);

	switch(toupper(key))
	{
	//case 'B': break;
	//case ' ':	break;
	case ' ':
	{
		break;
	}
	case 'B':
	{
		//canonballs.push_back(new Particle(, Vector3D(50, 0, 0), Vector3D(0, -9.8f, 0), 10.0f)); // Mass of 10.0 kilogram
		Camera* cam = GetCamera();
		if (cam)
		{
			// Posición y dirección de la cámara (physx::PxVec3)
			physx::PxVec3 eye = cam->getEye();
			physx::PxVec3 dir = cam->getDir(); // vector unitario hacia delante

			// Convertir a Vector3D (tu clase) y calcular velocidad inicial
			Vector3D spawnPos(eye.x, eye.y, eye.z);
			const float launchSpeed = 50.0f;
			Vector3D spawnVel(dir.x * launchSpeed, dir.y * launchSpeed, dir.z * launchSpeed);

			// Aceleración/gravedad y masa para la bala
			Vector3D gravity(0.0f, -9.8f, 0.0f);
			float mass = 10.0f;

			// Crear y almacenar la cannonball
			canonballs.push_back(new Particle(spawnPos, spawnVel, gravity, mass));
		}
		break;
	}
	default:
		break;
	}
}

void onCollision(physx::PxActor* actor1, physx::PxActor* actor2)
{
	PX_UNUSED(actor1);
	PX_UNUSED(actor2);
}


int main(int, const char*const*)
{
#ifndef OFFLINE_EXECUTION 
	extern void renderLoop();
	renderLoop();
#else
	static const PxU32 frameCount = 100;
	initPhysics(false);
	for(PxU32 i=0; i<frameCount; i++)
		stepPhysics(false);
	cleanupPhysics(false);
#endif

	return 0;
}