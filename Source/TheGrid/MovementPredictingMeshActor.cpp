// Fill out your copyright notice in the Description page of Project Settings.

#include "MovementPredictingMeshActor.h"

AStaticMeshActor* spawnMeshActor(UWorld* world, UStaticMesh* mesh) {
	AMovementPredictingMeshActor* meshActor = world->SpawnActor<AMovementPredictingMeshActor>(AMovementPredictingMeshActor::StaticClass());
	meshActor->GetStaticMeshComponent()->SetStaticMesh(mesh);
	meshActor->SetMobility(EComponentMobility::Movable);
	return meshActor;
}

AMovementPredictingMeshActor::AMovementPredictingMeshActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMovementPredictingMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMovementPredictingMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

