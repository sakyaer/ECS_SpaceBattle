// Fill out your copyright notice in the Description page of Project Settings.

#include "Battle_ECSWorld.h"
#include "ECS_Core.h"
#include "ECS_BaseSystems.h"
#include "ECS_BattleSystems.h"



// Sets default values
A_ECSWorldActor::A_ECSWorldActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void A_ECSWorldActor::BeginPlay()
{
	Super::BeginPlay();
	

		ECSWorld = MakeUnique<ECS_World>();
		TaskScheduler = MakeUnique<ECSSystemScheduler>();
		
		ECSWorld->CreateAndRegisterSystem<CopyTransformToECSSystem>();
		ECSWorld->CreateAndRegisterSystem<BoidSystem>();

		ECSWorld->CreateAndRegisterSystem<MovementSystem>();
		ECSWorld->CreateAndRegisterSystem<ExplosionSystem>();
		ECSWorld->CreateAndRegisterSystem<SpaceshipSystem>();
		ECSWorld->CreateAndRegisterSystem<RaycastSystem>();
		ECSWorld->CreateAndRegisterSystem<LifetimeSystem>();
		
		ECSWorld->CreateAndRegisterSystem<DebugDrawSystem>();

		
		ECSWorld->CreateAndRegisterSystem<StaticMeshDrawSystem>();

		ECSWorld->CreateAndRegisterSystem<CopyTransformToActorSystem>();

		ECSWorld->CreateAndRegisterSystem<ArchetypeSpawnerSystem>();		

		ECSWorld->InitializeSystems(this);
		
		
	
}
namespace ECSCVars
{
	// Listen server smoothing
	static int32 EnableParallel = 0;
	FAutoConsoleVariableRef CVarParallelECS(
		TEXT("p.parallelECS"),
		EnableParallel,
		TEXT("Whether to run the ECS task graph in multiple cores\n")
		TEXT("0: Disable, 1: Enable"),
		ECVF_Default);
}
// Called every frame
void A_ECSWorldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SCOPE_CYCLE_COUNTER(STAT_TotalUpdate);

	

	TaskScheduler->Reset();
	TaskScheduler->registry = &ECSWorld->registry;

	for(auto sys : ECSWorld->systems){
		 sys->schedule(TaskScheduler.Get());
	}

	TaskScheduler->Run(ECSCVars::EnableParallel == 1,ECSWorld->registry);
}

