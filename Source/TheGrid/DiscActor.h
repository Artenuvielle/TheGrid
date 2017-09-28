// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovementPredictingMeshActor.h"
#include "common.h"
#include "Observer.h"
#include "DiscActor.generated.h"

enum DiskState {
	DISK_STATE_READY = 0,
	DISK_STATE_DRAWN,
	DISK_STATE_FREE_FLY,
	DISK_STATE_RETURNING
};

UCLASS()
class THEGRID_API ADiscActor : public AActor, public Observable<GameNotifications>
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
	FVector getDiscMomentum();
	FQuat getDiscRotation();
	DiskState getState();

	bool startDraw(FVector pos);
	bool endDraw(FVector pos);
	bool forceReturn();
	bool forceThrow(FVector pos, FVector momentum);
	bool catchDisk();
private:
	FVector _discPosition;
	FQuat _discRotation;
	DiskState _state;
	FVector _momentum;
	FVector _lastPositionWhileDrawn;

	AStaticMeshActor* _discInnerActor;
	AStaticMeshActor* _discOuterActor;
	static UStaticMesh* _discMeshInner;
	static UStaticMesh* _discMeshOuter;
	static UMaterial* _blueMaterial;
	static UMaterial* _orangeMaterial;	
};
