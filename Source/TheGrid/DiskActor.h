// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovementPredictingMeshActor.h"
#include "common.h"
#include "Observer.h"
#include "NetworkPackets.h"
#include "DiskActor.generated.h"


UCLASS()
class THEGRID_API ADiskActor : public AActor, public Observable<GameNotifications>
{
	GENERATED_BODY()
	
public:	
	ADiskActor();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	void Init(PlayerFaction);

	void setPosition(FVector);
	void setRotation(FQuat);
	FVector getPosition();
	FVector getMomentum();
	FQuat getRotation();
	DiskState getState();

	bool startDraw(FVector pos);
	bool endDraw(FVector pos);
	bool forceReturn();
	bool forceThrow(FVector pos, FVector momentum);
	bool catchDisk();
private:
	FVector _diskPosition;
	FQuat _diskRotation;
	DiskState _state;
	FVector _momentum;
	FVector _lastPositionWhileDrawn;

	AStaticMeshActor* _diskMeshActor;
	static UStaticMesh* _diskMesh;
	static UMaterial* _blueMaterial;
	static UMaterial* _orangeMaterial;	
};
