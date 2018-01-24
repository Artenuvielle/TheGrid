// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovementPredictingMeshActor.h"
#include "DiskActor.h"
#include "ShieldActor.h"
#include "LifeCounterActor.h"
#include "common.h"
#include "PlayerActor.generated.h"

UCLASS()
class THEGRID_API APlayerActor : public AActor
{
	GENERATED_BODY()

public:
	APlayerActor();
	void Init(PlayerFaction, bool);

	void setFaction(PlayerFaction);
	PlayerFaction getFaction();

	void setHeadPosition(FVector);
	void setHeadRotation(FQuat);
	void setDiskArmPosition(FVector);
	void setDiskArmRotation(FQuat);
	void setShieldArmPosition(FVector);
	void setShieldArmRotation(FQuat);
	void setDrawState(bool);

	FVector getHeadPosition();
	FQuat getHeadRotation();
	FVector getDiskArmPosition();
	FQuat getDiskArmRotation();
	FVector getShieldArmPosition();
	FQuat getShieldArmRotation();
	ADiskActor* getDiskActor();
	AShieldActor* getShieldActor();
	ALifeCounterActor* getLifeCounterActor();
	bool getDrawState();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

private:
	void updatePositions();
	PlayerFaction _ownFaction;

	FVector _headPosition;
	FQuat _headRotation;
	FVector _diskArmPosition;
	FQuat _diskArmRotation;
	FVector _shieldArmPosition;
	FQuat _shieldArmRotation;
	FVector _torsoPosition;
	FQuat _torsoRotation;

	bool _drawModel;
	ADiskActor* _diskActor;
	AShieldActor* _shieldActor;
	ALifeCounterActor* _lifeCounterActor;
	AStaticMeshActor* _torsoActor;
	AStaticMeshActor* _headActor;
	AStaticMeshActor* _diskArmActor;
	AStaticMeshActor* _shieldArmActor;
	static UStaticMesh* _torsoMesh;
	static UStaticMesh* _headMesh;
	static UStaticMesh* _armMesh;
	static UMaterial* _blueMaterial;
	static UMaterial* _orangeMaterial;
};
