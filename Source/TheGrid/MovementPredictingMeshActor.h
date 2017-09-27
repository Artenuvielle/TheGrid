// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "MovementPredictingMeshActor.generated.h"

UCLASS()
class THEGRID_API AMovementPredictingMeshActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	AMovementPredictingMeshActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;	
};

template <typename T>
T* getContent(FString path) {
	ConstructorHelpers::FObjectFinder<T> meshFinder(*path);
	return meshFinder.Object;
}

AStaticMeshActor* spawnMeshActor(UWorld* world, UStaticMesh* mesh);