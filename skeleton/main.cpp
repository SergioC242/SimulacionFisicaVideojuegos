

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
#include "Boat.h"
#include "Boat2.h"
#include "SpringForceGenerator.h"
#include "BuoyancyForceGenerator.h"
#include "SolidRigid.h"
#include "ExplosionGenerator.h"


std::string display_text = "IS DEATH THE MEANING OF LIFE? NO ITS DELTARUNE CHAPTER 8";


using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;


PxMaterial* gMaterial = NULL;

PxPvd* gPvd = NULL;

PxDefaultCpuDispatcher* gDispatcher = NULL;
PxScene* gScene = NULL;
ContactReportCallback gContactReportCallback;

Particle* myParticle = nullptr;
//<Particle*> canonBall = nullptr;
//vector de punteros particle
std::vector<Particle*> canonballs;

//sistema de ñparticulas
ParticleSystem* Ps;
ParticleSystem* Ps2;

//Barco
Boat2* boat;


//Fisica SOLID RIGID
SolidRigid physics;

//Fo9rce generators
WindForceGenerator* wind1 = nullptr;
ExplosionGenerator* explosion = nullptr;
BuoyancyForceGenerator* b1 = nullptr;
BuoyancyForceGenerator* b2 = nullptr;
GravityForceGenerator* g1 = nullptr;


bool windActive = true;

static void generateSpringDemo() {
	// First one standard spring uniting 2 particles
	Particle* p1 = new Particle({ -10.0,10.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.85, 60);
	Particle* p2 = new Particle({ 10.0,10.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, 0.85, 60);
	p2->setMass(2.0);

	SpringForceGenerator* f1 = new SpringForceGenerator(p2, 1, 15);
	p1->addForceGenerator(f1);

	SpringForceGenerator* f2 = new SpringForceGenerator(p1, 1, 15);
	p2->addForceGenerator(f2);

	canonballs.push_back(p1);
	canonballs.push_back(p2);
}

static void generateSolids() {


	physics.initPhysics(gPhysics, gScene);

	// Crear suelo estático
	physics.createStaticBox(PxVec3(50, 15, -50), PxVec3(10));

	// Crear cubo dinámico
	physics.createDynamicBox(
		PxVec3(0, 20, -50),          // posición
		PxVec3(0.5f),           // mitad de cada lado
		0.15f                  // densidad
	);

	//Crear esfera dinámica
	physics.createDynamicSphere(
		PxVec3(5, 30, 0),    // posición
		0.5f,               // radio
		0.15f               // densidad
	);

	//g1 = new GravityForceGenerator(Vector3D(0.0f, -9.8f, 0.0f));
	//wind1 = new WindForceGenerator(Vector3D(50.0f, 0.0f, 0.0f));
	physics.addForceGenerator(wind1);
}

static void generateBuoyancyDemo()
{
	// Partícula representando el nivel del agua
	// Su posición Y determina la altura del líquido
	Particle* liquidLevel = new Particle(
		{ 0.0, 5.0, 0.0 },
		{ 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0 },
		1.0f,
		0.0f
	);

	// 2 partículas que flotarán
	Particle* p1 = new Particle(
		{ 0.0, 7.0, 0.0 },
		{ 0.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0 },
		1.0f,
		1.0f, { 1, 1, 0, 1 }
	);
	p1->setMass(1.0f);

	//Particle* p2 = new Particle(
	//	{ 0.0, 7.0, 0.0 },    
	//	{ 0.0, 0.0, 0.0 },
	//	{ 0.0, 0.0, 0.0 },
	//	0.85f,
	//	2.0f
	//);
	//p2->setMass(1.5f);



	//BuoyancyForceGenerator* b2 = new BuoyancyForceGenerator(3.0f, 1.0f, 1000.0f);
	b1 = new BuoyancyForceGenerator(1.0f, 1000.0f);
	g1 = new GravityForceGenerator(Vector3D(0.0f, -9.8f, 0.0f));

	// Indicamos quién marca el nivel del agua
	b1->setLiquidParticle(liquidLevel);
	//b2->setLiquidParticle(liquidLevel);

	// Asignamos generadores a las partículas
	p1->addForceGenerator(b1);
	p1->addForceGenerator(g1);
	//p2->addForceGenerator(b1);

	// Añadimos todo a la simulación
	canonballs.push_back(liquidLevel);
	canonballs.push_back(p1);
	//canonballs.push_back(p2);
}

static void generateBoatDemo()
{


	//wind
	Ps = new ParticleSystem(100.0f, 200.0f, 100.0f, 70.0f, Vector3D(0, -10, 0), Vector3D(0, 0, 0), 10.0f, 0.1f, { 0, 0.5, 1, 1 });

	//generate forces
	b1 = new BuoyancyForceGenerator(1.0f, 1000.0f);
	g1 = new GravityForceGenerator(Vector3D(0.0f, -9.8f, 0.0f));
	wind1 = new WindForceGenerator(Vector3D(50.0f, 0.0f, 0.0f));
	Ps->addForceGenerator(wind1);

	// create sea level
	// For where i spawn particles from -100 to 100 in x and z, and from 5 to 6 in y and all affected by gravity and buoyancy
	b2 = new BuoyancyForceGenerator(1.0f, 5.0f); // for consistency
	for (int i = 0; i < 50; i++)
	{
		float X = -i * 4 + 30;
		for (int j = 0; j < 50; j++)
		{
			float randomY = static_cast <float>(rand()) / (static_cast <float>(RAND_MAX / 1.0f));
			//sin wave from 0 to 2
			float waveY = sinf((i + j) * 0.4f) * 0.5f;
			float Z = -j * 4 + 30;
			Vector3D spawnPos = Vector3D(X, waveY + randomY + 3.5, Z);
			Vector3D initialVelocity = Vector3D(0, 0, 0);
			// Crear nueva partícula (Vector3D Pos, Vector3D Vel, Vector3D Accel, float mass, float lifespam, Vector4 col, float size)
			Particle* newParticle = new Particle(spawnPos, initialVelocity, Vector3D(0, 0, 0), 0.3f, -1.0f, Vector4(0, 0, 1, 1), 0.3f);
			newParticle->addForceGenerator(b2);
			newParticle->addForceGenerator(g1);
			canonballs.push_back(newParticle);
		}
	}
	//create boat
	boat = new Boat2({ 0, 6.0, 0 }, { 10, 0, 0 });



	boat->addForceGenerator(b1);
	boat->addForceGenerator(g1);
}

static void generateExplosionDemo() {
	Ps2 = new ParticleSystem(10.0f, 10.0f, 10.0f, 10.0f, Vector3D(0, 10, 0), Vector3D(0, 0, 0), 1.0f, 0.5f);
	explosion = new ExplosionGenerator(Vector3D(0.0f, 10.0f, 0.0f), 500.0f, 20.0f);
	Ps2->addForceGenerator(explosion);
}


// Initialize physics engine
void initPhysics(bool interactive)
{
	PX_UNUSED(interactive);

	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

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
	PxTransform* b = new PxTransform(0, 0, 0);
	Vector4 c = { 1, 1, 1, 1 };
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

	//Ps = new ParticleSystem(5.0f, 50.0f, Vector3D(0, 0, 0), Vector3D(-10, 0, -10), 10.0f);
	// OPCIÓN 2: Lluvia con viento (más realista)
	//Ps = new ParticleSystem(70.0f, 200.0f, 100.0f, 70.0f, Vector3D(0, -10, 0), Vector3D(0, 0, 0), 15.0f);
	//GravityForceGenerator* gravity1 = new GravityForceGenerator(Vector3D(0, -9.8f, 0));
	////Ps->addForceGenerator(gravity1);
	//
	//wind1 = new WindForceGenerator(Vector3D(50.0f, 0.0f, 0.0f));
	//Ps->addForceGenerator(wind1);

	gScene = gPhysics->createScene(sceneDesc);

	//boat = new Boat2({ 0, 0, 0 }, { 10, 0, 0 });

	//generateSpringDemo();
	//generateBuoyancyDemo();


	generateBoatDemo();
	generateSolids();
	//generateExplosionDemo();
}



// Function to configure what happens in each step of physics
// interactive: true if the game is rendering, false if it offline
// t: time passed since last call in milliseconds
void stepPhysics(bool interactive, double t)
{
	PX_UNUSED(interactive);

	gScene->simulate(t);
	gScene->fetchResults(true);

	//myParticle->integrate(t);
	if (Ps)
	{
		Ps->updateAll(t);
		Ps->integrateAll(t);
	}
	if (Ps2)
	{
		Ps2->updateAll(t);
		Ps2->integrateAll(t);
	}

	for (auto canonBall : canonballs)
		canonBall->integrate(t);
	if (boat)
	{
		boat->update(t);
		Camera* cam = GetCamera();

		//camera follow boat x, y ,z + offset 
		//if (cam)
		//{
		//	physx::PxVec3 boatPos = boat->getPos()->p;
		//	physx::PxVec3 offset = physx::PxVec3(0.0f, 5.0f, -15.0f); // Offset detrás y arriba del barco
		//	//cam->handleMotion(boatPos.x + offset.x, boatPos.y + offset.y);
		//	//cam->setDir(physx::PxVec3(boatPos.getX() - (boatPos.getX() + offset.getX()),
		//	//	boatPos.getY() - (boatPos.getY() + offset.getY()),
		//	//	boatPos.getZ() - (boatPos.getZ() + offset.getZ())).getNormalized());
		//} //No va por eso se lo comento
	}
	physics.stepPhysics(t);
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

	for (Particle* canonBall : canonballs)
	{
		delete canonBall;
	}

	if (Ps) delete Ps;
	if (boat) delete boat;

	//delete force generators
	if (wind1) delete wind1;
	if (b1) delete b1;
	if (g1) delete g1;
	if (explosion) delete explosion;
	if (b2) delete b2;

	gFoundation->release();
}

// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);

	switch (toupper(key))
	{
	case ' ':
	{
		break;
	}
	case 'B':
	{
		//boat->MoveForward();
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
	case 'Z':
	{
		// Toggle del viento
		//if (windActive)
		//{
		//	// Desactivar viento (establecer velocidad a 0)
		//	wind1->setWindVelocity(Vector3D(0.0f, 0.0f, 0.0f));
		//	windActive = false;
		//	std::cout << "Viento DESACTIVADO" << std::endl;
		//}
		//else
		//{
		//	// Activar viento (restaurar velocidad original)
		//	wind1->setWindVelocity(Vector3D(50.0f, 0.0f, 0.0f));
		//	windActive = true;
		//	std::cout << "Viento ACTIVADO" << std::endl;
		//}
		if (explosion) {
			explosion->trigger();
			std::cout << "Explosion triggered!" << std::endl;
		}
		break;
	}
	case 'I': {
		boat->largarVela(0.1f);
		break;
	}
	case 'K': {
		boat->cazarVela(0.1f);
		break;
	}
	case 'J': {
		boat->turnRight();
		break;
	}
	case 'L': {
		boat->turnLeft();
		break;
	}
	default:
		break;
	}
};

void onCollision(physx::PxActor* actor1, physx::PxActor* actor2)
{
	PX_UNUSED(actor1);
	PX_UNUSED(actor2);
}


int main(int, const char* const*)
{
#ifndef OFFLINE_EXECUTION 
	extern void renderLoop();
	renderLoop();
#else
	static const PxU32 frameCount = 100;
	initPhysics(false);
	for (PxU32 i = 0; i < frameCount; i++)
		stepPhysics(false);
	cleanupPhysics(false);
#endif

	return 0;
}