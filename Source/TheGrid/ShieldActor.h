// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DiskActor.h"
#include "ShieldActor.generated.h"

UCLASS()
class THEGRID_API AShieldActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShieldActor();
	void Init(PlayerFaction);
	virtual void Tick(float) override;

	void setEnemyDisk(ADiskActor*);
	void setPosition(FVector);
	FVector getPosition();
	void setRotation(FQuat);
	FQuat getRotation();
	float getRadius();
	void setCharges(int);
	int getCharges();

	void reduceCharges();
	void refillCharges();
	bool hasCharges();
private:
	ADiskActor* _enemyDisk;
	float _radius;
	int _charges;
	FVector _shieldPosition;
	FQuat _shieldRotation;

	AStaticMeshActor* _shieldMeshActor;
	static UStaticMesh* _shieldMesh;
	static UMaterial* _blueMaterial;
	static UMaterial* _orangeMaterial;
	static UMaterial* _blueMaterialOpaque;
	static UMaterial* _orangeMaterialOpaque;
};
