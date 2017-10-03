// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "common.h"
#include "NetworkPackets.h"
#include "AnimationActor.h"
#include "WallCollisionActor.generated.h"

UCLASS()
class THEGRID_API AWallCollisionActor : public AAnimationActor
{
	GENERATED_BODY()
	
public:
	AWallCollisionActor();
	void Init(PlayerFaction, FVector, float, CollisionWall);
	virtual void Tick(float DeltaTime) override;
protected:
	float _maxTime;
private:	
	void createAnimationsAtCollisionPoint(PlayerFaction, float, FVector, FVector, CollisionWall);
	float _time;
	static UMaterial* _blueCollisionMaterial;
	static UMaterial* _orangeCollisionMaterial;
};
