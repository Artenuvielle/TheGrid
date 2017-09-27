// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovementPredictingMeshActor.h"
#include "common.h"
#include "DiscActor.generated.h"

enum DiskState {
	DISK_STATE_READY = 0,
	DISK_STATE_DRAWN,
	DISK_STATE_FREE_FLY,
	DISK_STATE_RETURNING
};

UCLASS()
class THEGRID_API ADiscActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADiscActor();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	void Init(PlayerFaction);

	void setDiscPosition(FVector);
	void setDiscRotation(FQuat);
	FVector getDiscPosition();
	FQuat getDiscRotation();
	DiskState getState();

private:
	FVector _discPosition;
	FQuat _discRotation;
	DiskState _state;

	AStaticMeshActor* _discInnerActor;
	AStaticMeshActor* _discOuterActor;
	static UStaticMesh* _discMeshInner;
	static UStaticMesh* _discMeshOuter;
	static UMaterial* _blueMaterial;
	static UMaterial* _orangeMaterial;	
};
